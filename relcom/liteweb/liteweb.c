/*
 * WWW Hypertext Daemon with built-in interface to KIAE file archive.
 *
 * Copyright (C) 1994-1997 Cronyx Ltd.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 * Date: Thu Mar 17 18:56:45 MSK 1994
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <syslog.h>
#include <ndbm.h>
#include <libutil.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "reg.h"
#include "map.h"

#define LINESZ          512                     /* maximum input line length */
#define STACKSZ         10                      /* depth of if/endif */
#define TIMEOUT         60                      /* keepalive connection timeout */
#define ROOTDIR         "/pub"                  /* default root directory */
#define DBNAME          "/var/db/liteweb/user"    /* user database file name */
#define CONTENTS        "index.html"            /* directory contents file */
#define HTTPVERSION     "HTTP/1.0"              /* our version of protocol */
#define DATE822         "%a, %d %b %Y %T GMT"   /* RFC-822 date format */

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

#define ISSPACE(c)	((c) == ' ' || (c) == '\t')
#define ISDIGIT(c)	((c) >= '0' && (c) <= '9')

/*
 * HTTP status codes, from the HTTPv10-04 draft
 */
#define HS_OK                   200
#define HS_Created              201
#define HS_Accepted             202
#define HS_NoContent            204
#define HS_MultipleChoices      300
#define HS_MovedPermanently     301
#define HS_MovedTemporarily     302
#define HS_NotModified          304
#define HS_BadRequest           400
#define HS_Unauthorized         401
#define HS_Forbidden            403
#define HS_NotFound             404
#define HS_InternalServerError  500
#define HS_NotImplemented       501
#define HS_BadGateway           502
#define HS_ServiceUnavailable   503

#ifdef sun
#define strerror(e) sys_errlist[e]
extern char *sys_errlist[];
#else
extern char *strerror (int);
#endif

struct auth {
	struct auth *next;
	char *dir;
	int dirlen;                     /* dir name length for fast search */
	char *user;
	char *password;
	char *realm;
} *authlist;

struct forward {
	struct forward *next;
	char *dir;
	int dirlen;                     /* dir name length for fast search */
	char *host;
	unsigned long ipaddr;
	unsigned short port;
	char *dest;
	char *user;
	char *password;
	int flags;
	FILE *sockfd;
} *forwlist;
#define FW_PUBLIC       1
#define FW_KEEPALIVE    2

struct hometab {
	struct hometab *next;
	unsigned long ipaddr;
	char *hostname;
	char *homedir;
} homelist;

struct {
	char locator [LINESZ];          /* full locator */
	char search [LINESZ];           /* search part of the locator if any */
	char filepath [LINESZ];         /* file path with rootdir prepended */
	char dirpath [LINESZ];          /* directory part of filepath */
	char *reqname;                  /* requested file name (no rootdir) */
	char *basename;                 /* file name part of filepath */
	char *ext;                      /* extension part of the file name */
	unsigned trailing_slash : 1;    /* file path had trailing slashes */
} url;

enum {                                  /* HTTP protocol version */
	PROTO_0_9 = 9,                  /* 0.9 */
	PROTO_1_0 = 10,                 /* 1.0 */
	PROTO_1_1 = 11,                 /* 1.1 */
} proto;

typedef struct {
	char *name;
	char *path;
	struct stat st;
} filerec;

struct content_type {
	char *extension;
	int textual;
	char *icon;
	char *mimetype;
} content_tab [] = {
{ ".C",       1, "text",    "text/plain" },                /* C++ source           */
{ ".JPE",     0, "image2",  "image/jpeg" },
{ ".JPEG",    0, "image2",  "image/jpeg" },
{ ".JPG",     0, "image2",  "image/jpeg" },
{ ".MPE",     0, "movie",   "video/mpeg" },
{ ".MPEG",    0, "movie",   "video/mpeg" },
{ ".MPG",     0, "movie",   "video/mpeg" },                /* MPEG                 */
{ ".PS",      0, "pstscrpt","application/postscript" },    /* PostScript           */
{ ".S",       1, "text",    "text/plain" },
{ ".Z",       0, "compressed", "application/x-compress" }, /* Compressed data      */
{ ".z",       0, "compressed", "application/x-compress" }, /* Compressed data      */
{ ".ai",      0, "pstscrpt","application/postscript" },    /* Adobe Illustrator    */
{ ".aif",     0, "sound2",  "audio/x-aiff" },
{ ".aifc",    0, "sound2",  "audio/x-aiff" },
{ ".aiff",    0, "sound2",  "audio/x-aiff" },
{ ".arj",     0, "compressed", "application/x-arj" },
{ ".au",      0, "sound2",  "audio/basic" },
{ ".avi",     0, "movie",   "video/x-msvideo" },           /* MS Video for Windows */
{ ".bin",     0, "binary",  "application/octet-stream" },  /* Uninterpreted binary */
{ ".c",       1, "text",    "text/plain" },                /* C source             */
{ ".cc",      1, "text",    "text/plain" },                /* C++ source           */
{ ".cpio",    0, "tar",     "application/x-cpio" },        /* POSIX CPIO           */
{ ".csh",     1, "text",    "application/x-csh" },         /* C-shell script       */
{ ".dvi",     0, "binary",  "application/x-dvi" },         /* TeX DVI              */
{ ".eps",     0, "pstscrpt","application/postscript" },
{ ".etx",     1, "text",    "text/x-setext" },             /* Struct Enchanced Txt */
{ ".exe",     0, "binary",  "application/octet-stream" },
{ ".f90",     1, "text",    "text/plain" },                /* Fortran 90 source    */
{ ".gif",     0, "image2",  "image/gif" },                 /* GIF                  */
{ ".gz",      0, "compressed", "application/x-gzip" },     /* Compressed data      */
{ ".h",       1, "text",    "text/plain" },                /* C headers            */
{ ".hh",      1, "text",    "text/plain" },                /* C++ headers          */
{ ".htm",     1, "text",    "text/html" },                 /* HTML                 */
{ ".html",    1, "text",    "text/html" },                 /* HTML                 */
{ ".ief",     0, "image2",  "image/ief" },                 /* Image Exchange fmt   */
{ ".jpe",     0, "image2",  "image/jpeg" },
{ ".jpeg",    0, "image2",  "image/jpeg" },
{ ".jpg",     0, "image2",  "image/jpeg" },                /* JPEG                 */
{ ".latex",   1, "text",    "application/x-latex" },       /* LaTeX source         */
{ ".lzh",     0, "compressed", "application/x-lzh" },
{ ".m",       1, "text",    "text/plain" },                /* Objective-C source   */
{ ".man",     1, "text",    "application/x-troff-man" },   /* Troff with man macros*/
{ ".me",      1, "text",    "application/x-troff-me" },    /* Troff with me macros */
{ ".mov",     0, "movie",   "video/quicktime" },
{ ".movie",   0, "movie",   "video/x-sgi-movie" },         /* SGI "moviepalyer"    */
{ ".mpe",     0, "movie",   "video/mpeg" },
{ ".mpeg",    0, "movie",   "video/mpeg" },
{ ".mpg",     0, "movie",   "video/mpeg" },
{ ".mp2",     0, "audio",   "audio/mpeg" },
{ ".mp3",     0, "audio",   "audio/mpeg" },
{ ".m3u",     0, "audio",   "audio/mpeg-url" },
{ ".ms",      1, "text",    "application/x-troff-ms" },    /* Troff with ms macros */
{ ".pbm",     0, "image2",  "image/x-portable-bitmap" },   /* PBM Bitmap format    */
{ ".pdf",     0, "acrobat", "application/pdf" },
{ ".pgm",     0, "image2",  "image/x-portable-graymap" },  /* PBM Graymap format   */
{ ".pnm",     0, "image2",  "image/x-portable-anymap" },   /* PBM Anymap format    */
{ ".ppm",     0, "image2",  "image/x-portable-pixmap" },   /* PBM Pixmap format    */
{ ".ps",      0, "pstscrpt","application/postscript" },
{ ".qt",      0, "movie",   "video/quicktime" },           /* QuickTime            */
{ ".ras",     0, "image2",  "image/cmu-raster" },
{ ".rgb",     0, "image2",  "image/x-rgb" },
{ ".roff",    1, "text",    "application/x-troff" },
{ ".rtf",     0, "text",    "application/x-rtf" },         /* RTF                  */
{ ".rtx",     1, "text",    "text/richtext" },             /* MIME Richtext format */
{ ".s",       1, "text",    "text/plain" },
{ ".sh",      1, "text",    "application/x-sh" },          /* Shell-script         */
{ ".shar",    1, "text",    "application/x-shar" },        /* Shell archive        */
{ ".snd",     0, "sound2",  "audio/basic" },               /* Audio                */
{ ".t",       1, "text",    "application/x-troff" },       /* Troff                */
{ ".tar",     0, "tar",     "application/x-tar" },         /* 4.3BSD tar           */
{ ".tcl",     1, "text",    "application/x-tcl" },         /* TCL-script           */
{ ".tex",     1, "text",    "application/x-tex" },         /* TeX source           */
{ ".texi",    1, "text",    "application/x-texinfo" },     /* Texinfo              */
{ ".texinfo", 1, "text",    "application/x-texinfo" },
{ ".tgz",     0, "compressed", "application/x-gzip" },     /* Compressed tar */
{ ".tif",     0, "image2",  "image/tiff" },                /* TIFF                 */
{ ".tiff",    0, "image2",  "image/tiff" },
{ ".tr",      1, "text",    "application/x-troff" },
{ ".txt",     1, "text",    "text/plain" },                /* Plain text           */
{ ".wav",     0, "sound2",  "audio/x-wav" },               /* Windows+ WAVE format */
{ ".xbm",     1, "image2",  "image/x-xbitmap" },           /* X bitmap             */
{ ".xpm",     1, "image2",  "image/x-xpixmap" },           /* X pixmap format      */
{ ".xwd",     0, "image2",  "image/x-xwindowdump" },       /* X window dump (xwd)  */
{ ".zip",     0, "compressed", "application/zip" },           /* PKZIP                */
{ ".zoo",     0, "compressed", "application/x-zoo" },
{ 0,          0, 0 } };

/*
 * General headers
 */
char *h_date;
char *h_mime_version;
char *h_pragma;

/*
 * Request headers
 */
char *h_authorization;
char *h_from;
char *h_if_modified_since;
char *h_referer;
char *h_user_agent;

/*
 * Response headers
 */
char *h_location;
char *h_server;
char *h_www_authenticate;

/*
 * Entity headers
 */
char *h_allow;
char *h_content_encoding;
char *h_content_length;
char *h_content_type;
char *h_expires;
char *h_last_modified;
time_t h_modstamp;                      /* got from Last-Modified */
char *h_connection;                     /* nonstandard */

/*
 * CGI headers
 */
char *h_status;

/*
 * Authentication fields
 */
char *h_user;
char *h_password;

/*
 * Extended entity headers
 */
#define MAXH 32

struct xheader {
	char *name;
	char *value;
} h_xtab [MAXH];

int h_nxtab;

/*
 * Request body
 */
FILE *h_body;
long h_blen;

struct header {
	char *name;
	char **value;
} h_tab [] = {
	{ "date",               &h_date              },
	{ "mime-version",       &h_mime_version      },
	{ "pragma",             &h_pragma            },
	{ "authorization",      &h_authorization     },
	{ "from",               &h_from              },
	{ "if-modified-since",  &h_if_modified_since },
	{ "referer",            &h_referer           },
	{ "user-agent",         &h_user_agent        },
	{ "location",           &h_location          },
	{ "server",             &h_server            },
	{ "www-authenticate",   &h_www_authenticate  },
	{ "allow",              &h_allow             },
	{ "content-encoding",   &h_content_encoding  },
	{ "content-length",     &h_content_length    },
	{ "content-type",       &h_content_type      },
	{ "expires",            &h_expires           },
	{ "last-modified",      &h_last_modified     },
	{ "connection",         &h_connection        },
	{ "status",             &h_status            },
	{ 0 },
};

char *month[13] = { "Dec", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
			   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

const char VERSION[] = "LiteWeb/1.2";
const char COPYRIGHT[] = "Copyright (C) 1994-97 Cronyx Ltd.";

char *progname;
int verbose;
int debug;
int rusflag;

struct sockaddr_in my, peer;            /* server and client IP addresses */
char line [LINESZ];
char *inputptr;
char hostname [40];
char peername [40];
char *rootdir;
int reqcnt;                             /* count of requests per connection */
int keepalive;                          /* keep the connection alive */
struct stat filestat;
time_t now;                             /* time stamp of the request */
time_t langstamp;                       /* last language change */
FILE *reply;

int translate;				/* charset translation flag */
unsigned char touser[256];              /* from KOI8 to user charset */
unsigned char fromuser[256];            /* from user charset to KOI8 */

unsigned char trans_dos [128] = {
	0xc4, 0xb3, 0xda, 0xbf, 0xc0, 0xd9, 0xc3, 0xb4,
	0xc2, 0xc1, 0xc5, 0xdf, 0xdc, 0xdb, 0xdd, 0xde,
	0xb0, 0xb1, 0xb2, 0xf4, 0xfe, 0xf9, 0xfb, 0xf7,
	0xf3, 0xf2, 0xff, 0xf5, 0xf8, 0xfd, 0xfa, 0xf6,
	0xcd, 0xba, 0xd5, 0xf1, 0xd6, 0xc9, 0xb8, 0xb7,
	0xbb, 0xd4, 0xd3, 0xc8, 0xbe, 0xbd, 0xbc, 0xc6,
	0xc7, 0xcc, 0xb5, 0xf0, 0xb6, 0xb9, 0xd1, 0xd2,
	0xcb, 0xcf, 0xd0, 0xca, 0xd8, 0xd7, 0xce, 0xfc,
	0xee, 0xa0, 0xa1, 0xe6, 0xa4, 0xa5, 0xe4, 0xa3,
	0xe5, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae,
	0xaf, 0xef, 0xe0, 0xe1, 0xe2, 0xe3, 0xa6, 0xa2,
	0xec, 0xeb, 0xa7, 0xe8, 0xed, 0xe9, 0xe7, 0xea,
	0x9e, 0x80, 0x81, 0x96, 0x84, 0x85, 0x94, 0x83,
	0x95, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e,
	0x8f, 0x9f, 0x90, 0x91, 0x92, 0x93, 0x86, 0x82,
	0x9c, 0x9b, 0x87, 0x98, 0x9d, 0x99, 0x97, 0x9a,
};

unsigned char trans_win [128] = {
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xfe, 0xe0, 0xe1, 0xf6, 0xe4, 0xe5, 0xf4, 0xe3,
	0xf5, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee,
	0xef, 0xff, 0xf0, 0xf1, 0xf2, 0xf3, 0xe6, 0xe2,
	0xfc, 0xfb, 0xe7, 0xf8, 0xfd, 0xf9, 0xf7, 0xfa,
	0xde, 0xc0, 0xc1, 0xd6, 0xc4, 0xc5, 0xd4, 0xc3,
	0xd5, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce,
	0xcf, 0xdf, 0xd0, 0xd1, 0xd2, 0xd3, 0xc6, 0xc2,
	0xdc, 0xdb, 0xc7, 0xd8, 0xdd, 0xd9, 0xd7, 0xda,
};

unsigned char trans_mac [128] = {
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0xde, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0xdd, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0xfe, 0xe0, 0xe1, 0xf6, 0xe4, 0xe5, 0xf4, 0xe3,
	0xf5, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee,
	0xef, 0xdf, 0xf0, 0xf1, 0xf2, 0xf3, 0xe6, 0xe2,
	0xfc, 0xfb, 0xe7, 0xf8, 0xfd, 0xf9, 0xf7, 0xfa,
	0x9e, 0x80, 0x81, 0x96, 0x84, 0x85, 0x94, 0x83,
	0x95, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e,
	0x8f, 0x9f, 0x90, 0x91, 0x92, 0x93, 0x86, 0x82,
	0x9c, 0x9b, 0x87, 0x98, 0x9d, 0x99, 0x97, 0x9a,
};

/*
 * HTML extensions
 */
#define H_TEXT          1               /* text mode */
#define H_GRAPH         2               /* graphics mode */
#define H_TABLES        4               /* use tables */
int html;

/*
 * HTML extensions
 */
#define L_ENG           1               /* english language */
#define L_KOI8          2               /* russian koi8-r */
#define L_WIN           3               /* russian ms windows */
#define L_DOS           4               /* russian ms dos */
#define L_MAC           5               /* russian macintosh */
int lang;

/*
 * Client operating system
 */
#define OS_UNKNOWN      1
#define OS_X11          2
#define OS_MAC          3
#define OS_MSWINNT      4
#define OS_MSWIN95      5
#define OS_MSWIN        6
int os;

/*
 * HTTP methods
 */
#define M_GET           0
#define M_HEAD          1
#define M_POST          2
int method;
char *method_name[] = { "GET", "HEAD", "POST" };

extern char *optarg;
extern int optind;
extern int errno;
extern char **environ;
extern int getpeername (int, struct sockaddr *, int *);
extern time_t getdate (char *ctim);

void error (int c, char *m, ...);
int fgetinfo (char *info, int maxlen, char *rinfo, int rmaxlen,
	long *date, FILE *fd);

/*
 * Find a record in the user database.
 * The key consists of:
 * - our IP address, 4 bytes
 * - client IP address, 4 bytes
 * - client OS type, 1 byte
 * - client agent name, zero ended, up to 32 bytes
 * The value is:
 * - time stamp, 4 bytes
 * - user language, 1 byte
 * Return 0 if the value is not found.
 */
int getuser (unsigned long myaddr, unsigned long peeraddr, char *agent, int os,
	time_t *stamp)
{
	DBM *db;
	datum key, val;
	char buf[4+4+1+32];
	int ret;
	long lval;

	db = dbm_open (DBNAME, 0, 0644);
	if (! db) {
		if (debug)
			syslog (LOG_ERR, "cannot open %s", DBNAME);
		return (0);
	}
	memset (buf, 0, sizeof(buf));
	*(long*)(buf+0) = myaddr;
	*(long*)(buf+4) = peeraddr;
	*(char*)(buf+8) = os;
	strcpy (buf+9, agent);
	key.dptr = buf;
	key.dsize = 4 + 4 + 1 + strlen (agent) + 1;
	val = dbm_fetch (db, key);
	if (! val.dptr) {
		if (verbose > 1)
			syslog (LOG_INFO, "[%s] not in db: 0x%x 0x%x %d `%s'", peername,
				myaddr, peeraddr, os, agent);
		dbm_close (db);
		return (0);
	}
	memcpy (&lval, val.dptr, sizeof (long));
	*stamp = lval;
	ret = *(unsigned char*)(val.dptr+4);
	dbm_close (db);
	return ret;
}

/*
 * Find the first occurence of the string P in the string STR.
 * Ingnore the case when comparing.
 */
char *strcasestr (char *str, char *p)
{
	int len = strlen (p);

	for (; *str; ++str)
		if (strncasecmp (str, p, len) == 0)
			return (str);
	return (0);
}

int getstr (FILE *fd, char *str, int sz)
{
	char *beg = str;
	int c;

	c = getc (fd);
	if (c < 0)                              /* check EOF */
		return (0);
	ungetc (c, fd);
	while (--sz>0 && (c = getc (fd)) >= 0 && c != '\n')
		*str++ = c;
	if (str>beg && str[-1] == '\r')         /* trim trailing CR */
		--str;
	*str = 0;
	return (1);
}

/*
 * Get next word from input line.
 * Inputptr points to the beginning of line.
 */
char *getarg ()
{
	char *p, *s;

	p = inputptr;
	while (*p==' ' || *p=='\t')
		++p;
	if (! *p)
		return (0);
	s = p;
	while (*p && *p!=' ' && *p!='\t')
		++p;
	if (*p)
		*p++ = 0;
	inputptr = p;
	return (s);
}

/*
 * Unescape URL special characters.
 */
void unescape (char *path)
{
	char buf [LINESZ], *s = buf, *p;
	unsigned char v;

	for (p=path; *p; ++p) {
		if (*p == '+') {
			*s++ = ' ';
			continue;
		}
		if (*p != '%') {
			*s++ = *p;
			continue;
		}
		v = *++p;
		if (v <= '9')
			*s = (v & 15) << 4;
		else
			*s = ((v & 7) + 9) << 4;
		v = *++p;
		if (v <= '9')
			*s++ |= (v & 15);
		else
			*s++ |= (v & 7) + 9;
	}
	*s = 0;
	strcpy (path, buf);
}

/*
 * Replace special chars in URL path with %XX escapes.
 */
void escape (char *path)
{
	char buf [LINESZ], *s = buf, *p;

	for (p=path; *p; ++p) {
		switch (*p) {
		case '"':  case '&':  case '(':  case ')':
		case ',':  case ';':  case '\'':
		case '{':  case '}':  /*case '=':*/
			*s++ = '%';
			*s++ = "0123456789ABCDEF" [*p>>4 & 15];
			*s++ = "0123456789ABCDEF" [*p & 15];
			continue;
		}
		*s++ = *p;
	}
	*s = 0;
	strcpy (path, buf);
}

/*
 * Replace special HTML characters <, >, &.
 */
void canon (char *name)
{
	char buf [LINESZ], *s = buf, *p;

	s = buf;
	for (p=name; *p; ++p) {
		switch (*p) {
		case '&':
			strcpy (s, "&amp;");
			s += 5;
			continue;
		case '<':
			strcpy (s, "&lt;");
			s += 4;
			continue;
		case '>':
			strcpy (s, "&gt;");
			s += 4;
			continue;
		}
		*s++ = *p;
	}
	*s = 0;
	strcpy (name, buf);
}

void printdate (char *name, time_t t)
{
	struct tm *ptm = gmtime (&t);
	char dat[80];

	strftime (dat, sizeof (dat), DATE822, ptm);
	printf ("%s: %s\r\n", name, dat);
}

char *mergename (char *parent, char *child)
{
	char *r, *q;

	r = malloc (strlen (parent) + strlen (child) + 2);
	if (! r)
		return (0);
	strcpy (r, parent);
	if (*child == '/') {
		r [strlen (rootdir)] = 0;
		++child;
	} else if ((q = strrchr (r, '/')) && q)
		*q = 0;
	for (; *child; child=q) {
		q = strchr (child, '/');
		if (! q) {
			strcat (r, "/");
			strcat (r, child);
			return r;
		}
		*q++ = 0;
		if (! *child || strcmp (child, ".") == 0)
			continue;
		if (strcmp (child, "..") == 0) {
			char *s = strrchr (r, '/');
			if (! s)
				continue;
			while (s>r && s[-1]=='/')
				--s;
			*s = 0;
			continue;
		}
		strcat (r, "/");
		strcat (r, child);
	}
	strcat (r, "/");
	return r;
}

/*
 * Get the name from the file.  The name is ended by space or "--".
 * If end of comment ("--") found, then assign 0 to inside_comment flag.
 * Return the pointer to internal static buffer, or 0 if no name got.
 */
char *getname (FILE *from, int *inside_comment)
{
	static char name[128];
	char *p = name;
	int c;

	while ((c = getc (from)) == ' ' || c == '\t')
		continue;
	for (;;) {
		if (c < 0200 && ! (c >= 'a' && c <= 'z') &&
		    ! (c >= 'A' && c <= 'Z') && ! (c >= '0' && c <= '9') &&
		    c != '-' && c != '/' && c != '.' && c != '_') {
			ungetc (c, from);
			break;
		}
		if (c == '-') {
			int n = getc (from);
			if (n == '-') {
				*inside_comment = 0;
				break;
			}
			ungetc (n, from);
		}
		if (p < name+sizeof(name)-1)
			*p++ = c;
		c = getc (from);
	}
	*p = 0;
	if (! *name)
		return (0);
	return (name);
}

/*
 * Skip all up to "--".
 */
void getcomment (FILE *from)
{
	int c;

	while ((c = getc (from)) >= 0)
		if (c == '-') {
			c = getc (from);
			if (c == '-')
				break;
			ungetc (c, from);
		}
}

int getexpr (FILE *from, int *inside_comment)
{
	char *p;
	int c;

	while ((c = getc (from)) == ' ' || c == '\t')
		continue;
	if (c < 0)
		return (-1);
	if (c == '!') {
		c = getexpr (from, inside_comment);
		if (c < 0)
			return (-1);
		return (! c);
	}
	ungetc (c, from);
	p = getname (from, inside_comment);
	if (! p)
		return (-1);
	if (strcasecmp (p, "TEXT") == 0)        return html & H_TEXT;
	else if (strcasecmp (p, "GRAPH") == 0)  return html & H_GRAPH;
	else if (strcasecmp (p, "TABLES") == 0) return html & H_TABLES;
	else if (strcasecmp (p, "ENG") == 0)    return lang == L_ENG;
	else if (strcasecmp (p, "KOI8") == 0)   return lang == L_KOI8;
	else if (strcasecmp (p, "WIN") == 0)    return lang == L_WIN;
	else if (strcasecmp (p, "DOS") == 0)    return lang == L_DOS;
	else if (strcasecmp (p, "MAC") == 0)    return lang == L_MAC;
	return (-1);
}

/*
 * Parse the HTML file, and process preprocessor statements:
 * - Conditional statement: #if EXPR - #elif - #else - #endif
 * - Include statement: #include FILENAME
 */
void preprocess (FILE *from, FILE *to, char *myname)
{
	int stack [STACKSZ], top = -1;
	int c, val, inside_comment;
	char *p;

	fseek (from, 0L, 0);
	while ((c = getc (from)) >= 0) {
		switch (c) {
		default:
			if (top < 0 || stack[top] > 0)
				putc (c, to);
			continue;
		case '\n':
			/* End of line -- add CR/LF */
			if (top < 0 || stack[top] > 0)
				fputs ("\r\n", to);
			continue;
		case '\r':
			/* Carriage return -- ignore */
			continue;
		case '<':
			/* HTML tag -- find "<!--#" */
			if ((c = getc (from)) != '!') {
				if (top < 0 || stack[top] > 0) {
					putc ('<', to);
					putc (c, to);
				}
				continue;
			}
			if ((c = getc (from)) != '-') {
				if (top < 0 || stack[top] > 0) {
					fputs ("<!", to);
					putc (c, to);
				}
				continue;
			}
			if ((c = getc (from)) != '-') {
				if (top < 0 || stack[top] > 0) {
					fputs ("<!-", to);
					putc (c, to);
				}
				continue;
			}
			while ((c = getc (from)) == ' ' || c == '\t')
				continue;
			if (c != '#') {
				if (top < 0 || stack[top] > 0) {
					fputs ("<!--", to);
					putc (c, to);
				}
				continue;
			}
			break;
		}

		/* Preprocessor statement found. */
		inside_comment = 1;
		p = getname (from, &inside_comment);
		if (! p)
			fprintf (to, "<!--Invalid empty directive-->\r\n");
		else if (strcasecmp (p, "dir") == 0)
			fputs (url.dirpath + strlen(rootdir), to);
		else if (strcasecmp (p, "path") == 0)
			fputs (url.filepath + strlen(rootdir), to);
		else if (strcasecmp (p, "if") == 0) {
			if (! inside_comment ||
			    (val = getexpr (from, &inside_comment)) < 0) {
				fputs ("<!--Invalid #if expression-->\r\n", to);
				getcomment (from);
			} else if (top >= STACKSZ-1)
				fputs ("<!--Too deep #if statement-->\r\n", to);
			else if (top >= 0 && stack[top] < 1)
				stack[++top] = -1;
			else
				stack[++top] = val;
		} else if (strcasecmp (p, "elif") == 0) {
			if (! inside_comment ||
			    (val = getexpr (from, &inside_comment)) < 0) {
				fputs ("<!--Invalid #elif expression-->\r\n", to);
				getcomment (from);
			} else if (top < 0)
				fputs ("<!--Incorrect #elif without #if-->\r\n", to);
			else if (stack[top] > 0)
				stack[top] = -1;
			else if (stack[top] == 0)
				stack[top] = val;
		} else if (strcasecmp (p, "endif") == 0) {
			if (top < 0)
				fputs ("<!--Incorrect #endif without #if-->\r\n", to);
			else
				--top;
		} else if (strcasecmp (p, "else") == 0) {
			if (top < 0)
				fputs ("<!--Incorrect #else without #if-->\r\n", to);
			else if (stack[top] >= 0)
				stack[top] = ! stack[top];
		} else if (strcasecmp (p, "include") == 0) {
			if (! inside_comment ||
			    ! (p = getname (from, &inside_comment)))
				fputs ("<!--Invalid #include file name-->\r\n", to);
			else if (top < 0 || stack[top] > 0) {
				p = mergename (myname, p);
				if (! p)
					fprintf (to, "<!--Out of memory in ##include-->\r\n");
				else {
					FILE *fd = fopen (p, "r");
					if (! fd)
						fprintf (to, "<!--Cannot ##include %s-->\r\n",
							p + strlen (rootdir));
					else {
						fprintf (to, "<!--Include file %s-->\r\n",
							p + strlen (rootdir));
						preprocess (fd, to, p);
						fprintf (to, "<!--Back to file %s-->\r\n",
							myname + strlen (rootdir));
						fclose (fd);
					}
					free (p);
				}
			}
		} else
			fprintf (to, "<!--Invalid directive: #%s-->\r\n", p);

		/* Get end of tag.  Skip all comments. */
		if (inside_comment)
			getcomment (from);
		while ((c = getc (from)) >= 0 && c != '>')
			if (c == '-')
				if ((c = getc (from)) == '-')
					getcomment (from);
				else
					ungetc (c, from);

		/* Skip spaces/newlines after preprocessor directive. */
		while ((c = getc (from)) >= 0 && (c == ' ' || c == '\t' ||
		    c == '\r' || c == '\n' || c == '\f'))
			continue;
		if (c >= 0)
			ungetc (c, from);
	}
}

void copy (FILE *from, FILE *to, unsigned long len, int transflag)
{
	unsigned char *tab = 0;

	if (transflag > 0)
		tab = touser;
	else if (transflag < 0)
		tab = fromuser;
	fseek (from, 0L, 0);
	while (len-- > 0) {
		int c = getc (from);
		if (c < 0)
			break;
		if (tab) {
			if (c == '%') {
				if (len-- <= 0)
					break;
				c = getc (from);
				if (c < 0)
					break;
				putc ('%', to);
				if ((c >= 'A' && c <= 'F') ||
				    (c >= 'a' && c <= 'f') ||
				    (c >= '0' && c <= '9')) {
					int d;
					if (len-- <= 0)
						break;
					d = getc (from);
					if (d < 0)
						break;
					if      (c >= 'A' && c <= 'F') c -= 'A' - 10;
					else if (c >= 'a' && c <= 'f') c -= 'f' - 10;
					else                           c &= 0x0f;
					if      (d >= 'A' && d <= 'F') d -= 'A' - 10;
					else if (d >= 'a' && d <= 'f') d -= 'f' - 10;
					else                           d &= 0x0f;
					c = tab [c<<4 | d];
					putc ("0123456789ABCDEF" [c>>4], to);
					putc ("0123456789ABCDEF" [c&15], to);
					continue;
				}
			}
			c = tab[c];
		}
		putc (c, to);
	}
}

/*
 * Create and open a temporary (noname) file.
 * Return the descriptor.  The file will be deleted on close.
 */
FILE *ftemp ()
{
	static char *name, pattern[] = "/tmp/lhtXXXXXX";
	FILE *fd;

	if (! name)
		name = (char*) mktemp (pattern);
	fd = fopen (name, "w+");
	if (! fd)
		return (0);
	unlink (name);
	return (fd);
}

void mk_reply ()
{
	if (reply)
		fseek (reply, 0L, 0);
	else {
		reply = ftemp ();
		if (! reply)
			error (HS_InternalServerError,
				"cannot make temporary file");
	}
}

void printtype (char *type, int textual)
{
	char *charset = 0;

	/* Some Web browsers (Lynx) do not understand the charset parameter
	 * of the Content-Type field... */
#if notyet
	if (textual && strcmp (type, "text/html") == 0)
		switch (lang) {
		case L_KOI8: charset = "koi8-r";    break;
		case L_WIN:  charset = "x-cp866";   break;
		case L_DOS:  charset = "x-cp850";   break;
		case L_MAC:  charset = "x-mac-cyr"; break;
		}
#endif
	printf ("Content-Type: %s", type);
	if (charset)
		printf ("; charset=%s", charset);
	printf ("\r\n");
}

void put_reply ()
{
	long len = ftell (reply);

	printf ("Content-Length: %ld\r\n\r\n", len);
	copy (reply, stdout, len, translate);
}

void fatal (int code, char *msg)
{
	char header[256];
	static const char footer[] = "\r\n</body>\r\n";

	sprintf (header, "<head><title>Error %3d: %s</title></head>\r\n"
			"<body><h1>Error %3d</h1>\r\n"
			"<b>Reason: </b>",
		code, msg, code);
	if (proto != PROTO_0_9) {
		printf ("%s %03d %s\r\n", HTTPVERSION, code, msg);
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		printdate ("Date", now);
		printtype ("text/html", 0);
		printf ("Content-Length: %d\r\n\r\n", (int)
			(strlen(header) + strlen(msg) + strlen(footer)));
	}
	fputs (header, stdout);
	fputs (msg, stdout);
	fputs (footer, stdout);
	fflush (stdout);
	exit (-1);
}

void error (int c, char *m, ...)
{
	va_list ap;
	char logmsg [LINESZ];

	va_start (ap, m);
	vsyslog (LOG_ERR, m, ap);
	vsprintf (logmsg, m, ap);
	va_end (ap);
	fatal (c, logmsg);
}

/*
 * The full HTTP locator syntax:
 * http: // [ user [:password] @ ] host [:port] /dir/subdir/file.ext ? search
 * \__/     \____________________/ \__________/ \__________________/
 * naming           access             host           file path
 * scheme           rights          parameters
 *
 * We don't want to perform gateway functions, so we restrict
 * it to the minimal subset: "http:/dir/subdir/file.ext?search".
 */
void parse ()
{
	char *s, *p = url.locator;

	/* check the naming scheme `http:' */
	s = strchr (p, ':');
	if (s) {
		if (strncmp (p, "http:", 5) != 0)
			error (HS_BadRequest, "invalid naming scheme `%s'", p);
		p = s+1;
	}
#if 0
	if (p[0]!='/' || p[1]=='/')
		error (HS_BadRequest, "invalid file path `%s'", p);
#else
	if (p[0] != '/')
		error (HS_BadRequest, "invalid file path `%s'", p);
	if (p[1] == '/') {
		/* remove the host name */
		s = strchr (p+2, '/');
		if (s)
			p = s;
		else
			p[1] = 0;
	}
#endif
	strcpy (url.filepath, rootdir);
	url.reqname = url.filepath + strlen (url.filepath);

	/* translate to internal charset */
	if (translate)
		for (s=p; *s; ++s)
			*s = fromuser[(unsigned char)*s];

	/* extract the search part */
	s = strchr (p, '?');
	if (s) {
		*s = 0;
		strcpy (url.search, s+1);
		strcat (url.filepath, p);
		*s = '?';
	} else {
		url.search[0] = 0;
		strcat (url.filepath, p);
	}

	/* remove trailing slashes */
	p = url.filepath + strlen (rootdir);
	url.trailing_slash = 0;
	for (;;) {
		s = strrchr (p, '/');           /* always successful */
		if (!s || s<=p || s[1])
			break;
		url.trailing_slash = 1;
		*s = 0;
	}

	unescape (url.filepath);
	unescape (url.search);

	s = strrchr (url.filepath, '/');        /* always successful */
	*s = 0;
	strcpy (url.dirpath, url.filepath);
	*s = '/';
	url.basename = s+1;

	url.ext = strrchr (url.basename, '.');
	if (! url.ext)
	        url.ext = "";
}

void interrupt ()
{
	error (HS_InternalServerError, "interrupted");
}

/*
 * Check if the file contains pure text.
 */
int textfile (FILE *fd)
{
	int c, n;

	for (n=0; n<1024; ++n) {
		c = getc (fd);
		if (c < 0)
			break;
		if (c == 0177)
			return (0);
		if (c >= ' ')
			continue;
		switch (c) {    /* Legal text characters: */
		case '\32':     /* ^Z */
		case '\r':      /* CR */
		case '\n':      /* NewLine */
		case '\b':      /* BackSpace */
		case '\t':      /* Tab */
		case '\f':      /* FormFeed */
			continue;
		}
		return (0);
	}
	return (1);
}

char *content_type (FILE *fd, int *txt, char *extension)
{
	struct content_type *p;

	if (extension && *extension)
		for (p=content_tab; p->extension; ++p)
			if (strcmp (p->extension, extension) == 0) {
				*txt = p->textual;
				return (p->mimetype);
			}
	*txt = textfile (fd);
	fseek (fd, 0, 0L);
	return (*txt ? "text/plain" : "application/octet-stream");
}

int filecmp (filerec *a, filerec *b)
{
	int dira = ((a->st.st_mode & S_IFMT) == S_IFDIR);
	int dirb = ((b->st.st_mode & S_IFMT) == S_IFDIR);
	if (dira != dirb)
		return (dira ? -1 : 1);
	return (strcmp (a->name, b->name));
}

unsigned char lower (unsigned char c)
{
	if (c>='A' && c<='Z')
		return (c + 'a' - 'A');
	if (c>=0340 && c<=0377)
		return (c - 040);
	if (c==0263)
		return (0243);
	return (c);
}

char *strlower (char *str)
{
	char *p;

	for (p=str; *p; ++p)
		*p = lower (*p);
	return (str);
}

char *icon (char *path)
{
	struct content_type *p;
	char *extension = strrchr (path, '.');
	int txt;
	FILE *fd;

	if (extension)
		for (p=content_tab; p->extension; ++p)
			if (strcmp (p->extension, extension) == 0)
				return p->icon;
	fd = fopen (path, "r");
	if (! fd)
		return "unknown";
	txt = textfile (fd);
	fclose (fd);
	return txt ? "text" : "binary";
}

void send_file ()
{
	struct tm *ptm;
	FILE *fd;
	char *type;
	int textual;
	time_t dmod = filestat.st_mtime;
	unsigned long size;

	if (dmod < langstamp)
		dmod = langstamp;
	if (dmod <= h_modstamp) {
not_modified:   printf ("%s 304 Document not modified\r\n", HTTPVERSION);
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		if (keepalive)
			printf ("Connection: Keep-Alive\r\n");
		printf ("Content-Length: 0\r\n");
		printdate ("Date", now);
		printf ("\r\n");
		syslog (LOG_INFO, "[%s] file %s not modified",
			peername, url.filepath);
		return;
	}

	fd = fopen (url.filepath, "r");
   	if (! fd)
		error (HS_Forbidden, "cannot read file `%s'", url.filepath);

	size = filestat.st_size;
	type = content_type (fd, &textual, url.ext);
	if (! textual && filestat.st_mtime <= h_modstamp)
		goto not_modified;
	if (textual && strcmp (url.ext, ".html") == 0) {
		mk_reply ();
		preprocess (fd, reply, url.filepath);
		size = ftell (reply);
	}
	if (proto != PROTO_0_9) {
		char dat [60];

		printf ("%s 200 Document follows\r\n", HTTPVERSION);
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		if (keepalive)
			printf ("Connection: Keep-Alive\r\n");
		printdate ("Date", now);
		ptm = gmtime (&dmod);
		strftime (dat, sizeof (dat), DATE822, ptm);
		printf ("Last-Modified: %s\r\n", dat);
		printtype (type, textual);
		printf ("Content-Length: %lu\r\n\r\n", size);

		if (method == M_HEAD) {
			syslog (LOG_INFO, "[%s] sent header of %s file %s",
				peername, textual ? "text" : "binary",
				url.filepath + strlen(rootdir));
			fclose (fd);
			return;
		}
	}

	if (proto == PROTO_0_9 && strcmp (url.ext, ".txt") == 0)
		printf ("<PLAINTEXT>\r\n");

	if (! textual)
		copy (fd, stdout, size, 0);
	else if (strcmp (url.ext, ".html") != 0)
		copy (fd, stdout, size, translate);
	else
		copy (reply, stdout, size, translate);

	fclose (fd);

	syslog (LOG_INFO, "[%s] sent %s file %s total %lu bytes",
		peername, textual ? "text" : "binary",
		url.filepath + strlen(rootdir), size);
}

/*
 * Perform the search in the directory index file.
 * The index file contains one line per each file.
 * The format is simple:
 *     <file path> description
 */
/*
 * Add the header/footer to the directory.
 * Header of the directory /a/b/c is placed in the file /a/.header,
 * footer - in the file /a/.footer.
 */
int putsection (FILE *reply, char *filename)
{
	FILE *fd;
	char name[LINESZ], *p;

	strcpy (name, url.filepath);
	p = name + strlen (rootdir);
	while (*p == '/')
		++p;
	p = strchr (p, '/');
	if (p)
		*++p = 0;
	else
		strcat (name, "/");
	strcat (name, filename);

	fd = fopen (name, "r");
	if (! fd)
		return (0);
	preprocess (fd, reply, name);
	fclose (fd);
	return (1);
}

void searchfile ()
{
	char info [LINESZ], descr [LINESZ], file [LINESZ];
	char sect [LINESZ], newsect [LINESZ], *s;
	char *search, *path, *p, *f;
	struct tm *ptm;
	struct stat st;
	REGEXP reg;
	FILE *fd;
	long size;
	int i, dirflag;

	search = url.search;
	if (strncmp (search, "search=", 7) == 0)
		search += 7;
	strlower (search);
	reg = REGCOMP (search);
	if (! reg)
		error (HS_BadRequest, "invalid regular expression `%s'",
			search);

	strcpy (info, url.filepath);
	strcat (info, "/.index");
	fd = fopen (info, "r");
	if (! fd)
		error (HS_NotFound, "no index file in directory `%s'",
			url.filepath);

	if (proto != PROTO_0_9) {
		char dat [60];

		printf ("%s 200 Document follows\r\n", HTTPVERSION);
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		if (keepalive)
			printf ("Connection: Keep-Alive\r\n");
		printdate ("Date", now);
		fstat (fileno (fd), &filestat);
		ptm = gmtime (&filestat.st_mtime);
		strftime (dat, sizeof (dat), DATE822, ptm);
		printf ("Last-Modified: %s\r\n", dat);
		printtype ("text/html", 1);

		if (method == M_HEAD) {
			printf ("\r\n");
			fclose (fd);
			syslog (LOG_INFO, "[%s] sent header of directory %s",
				peername, url.filepath + strlen(rootdir));
			return;
		}
	}

	mk_reply ();
	fprintf (reply, "<head><title>%s</title></head>\r\n",
		lang == L_ENG ? "Results of search" : "Результаты поиска");
	if (! putsection (reply, ".header.inc"))
		fprintf (reply, "<body>\r\n");

	fprintf (reply, "<h1>%s</h1><dl compact>\r\n",
		lang == L_ENG ? "Results of search" : "Результаты поиска");

	i = 0;
	*sect = 0;
	while (fgets (info, sizeof (info), fd)) {
		p = info + strlen (info);
		while (p>info && (p[-1]=='\n' || p[-1]=='\r'))
			*--p = 0;

		path = info;
		p = strchr (info, ' ');
		if (p) {
			*p++ = 0;
			p = strchr (p, ' ');
		}
		if (! p)
			p = "";

		f = strrchr (path, '/');
		dirflag = 0;
		if (! f)                /* base file name */
			f = path;
		else if (! f[1]) {      /* directory */
			dirflag = 1;
			while (f>path && *f=='/')
				*f-- = 0;
			while (f>path && f[-1]!='/')
				--f;
		} else                  /* file path */
			++f;

		strcpy (file, f);
		strcpy (descr, p);
		strlower (file);
		strlower (descr);
		if (!REGEXEC (reg, file) &&
		    (!*descr || !REGEXEC (reg, descr)))
			continue;

		strcpy (file, url.filepath);
		strcat (file, "/");
		strcat (file, path);
		if (stat (file, &st) >= 0 &&
		    (st.st_mode & S_IFMT) == S_IFREG)
			size = st.st_size;
		else
			size = -1;

		if (i == 0)
			fprintf (reply, "<hr>\r\n");

		/* Get section name. */
		s = path;
		while (strncmp (s, "../", 3) == 0)
			s += 3;
		s = strchr (s, '/');
		if (! s)
			s = path + strlen (path);
		strncpy (newsect, path, s-path);
		newsect[s-path] = 0;
		if (strcmp (sect, newsect) != 0) {
			strcpy (sect, newsect);
			fprintf (reply, "</dl><h3>%s</h3><dl compact>\r\n",
				sect);
		}

		if (dirflag)
			strcat (path, "/");
		fprintf (reply, "<dt><a href=\"%s\">%s</a>", path, path);
		if (size >= 0) {
			ptm = localtime (&st.st_mtime);
			fprintf (reply, " -- %ld bytes, %ld", size,
				ptm->tm_year * 10000L + (ptm->tm_mon + 1) * 100 +
				ptm->tm_mday);
		}
		fprintf (reply, "\r\n");
		if (*p)
			fprintf (reply, "<dd>%s\r\n", p);
		++i;
	}
	fclose (fd);
	fprintf (reply, "</dl>\r\n");
	if (i)
		putsection (reply, ".footer.inc");
	else
		fprintf (reply, "%s.\r\n", lang == L_ENG ?
			"Search produced no result" : "Объект не найден");
	fprintf (reply, "</body>\r\n");
	put_reply ();
	syslog (LOG_INFO, "[%s] searched %s by `%s'",
		peername, url.filepath, search);
}

void printinfo (FILE *fd, char *s, int off, int indent)
{
	int c, n;
	char *p;

	n = 79 - indent;
	if (n < 0) {
		fputs ("\r\n", fd);

		for (n=0; n<off; ++n)
			putc (' ', fd);
		n = 74-off;
	}
	while (*s==' ' || *s=='\t')             /* skip leading blanks */
		++s;
	for (p=s; *p; ++p)
		if (*p == '\t')                 /* convert tabs to spaces */
			*p = ' ';
	for (--p; p>=s && *p==' '; --p)         /* remove trailing blanks */
		*p = 0;
	while (strlen (s) > n) {
		c = s [n];
		s [n] = 0;
		p = strrchr (s, ' ');
		s [n] = c;
		if (! p)
			p = s + n;

		c = *p;
		*p = 0;
		fputs (s, fd);
		fputs ("\r\n", fd);
		*p = c;

		for (n=0; n<off; ++n)
			putc (' ', fd);
		n = 74-off;

		s = p;
		while (*s == ' ')
			++s;
	}
	fputs (s, fd);
	fputs ("\r\n", fd);
}

void senddir ()
{
	char info [LINESZ], rinfo [LINESZ], path [LINESZ], name [LINESZ];
	char dat[60], *iptr;
	FILE *fd;
	DIR *dd;
	struct tm *ptm;
	struct dirent *d;
	filerec *filetab;
	int filetabsz, nfile, ndir, i;
	struct stat ist;
	unsigned long mod;
	time_t dmod;

	if (*url.search) {
		searchfile ();
		return;
	}

	if (url.filepath[strlen(url.filepath)-1] == '/')
		sprintf (info, "%s%s", url.filepath, CONTENTS);
	else
		sprintf (info, "%s/%s", url.filepath, CONTENTS);
	if (access (info, 4) == 0) {
		strcpy (url.filepath, info);
		url.ext = ".html";
		url.trailing_slash = 0;
		stat (url.filepath, &filestat);
		send_file ();
		return;
	}

	/* Compute the directory last modified time. */
	dmod = filestat.st_mtime;
	sprintf (info, "%s/.info", url.filepath);
	if (stat (info, &ist) >= 0 && ist.st_mtime > dmod)
		dmod = ist.st_mtime;

	dd = opendir (url.filepath);
	if (! dd)
		error (HS_NotFound, "cannot read directory `%s'", url.filepath);

	filetabsz = 32;
	filetab = (filerec*) malloc (filetabsz * sizeof (filerec));
	if (! filetab)
		error (HS_InternalServerError, "no memory for filetab");
	nfile = 0;
	ndir = 0;

 	for (d=readdir(dd); d; d=readdir(dd)) {
		if (nfile >= filetabsz) {
			filetabsz += 32;
			filetab = (filerec*) realloc ((void*) filetab,
				filetabsz * sizeof (filerec));
			if (! filetab)
				error (HS_InternalServerError,
					"no memory for filetab");
		}
		if (d->d_name[0] == '.')
			continue;
		strcpy (path, url.filepath);
		strcat (path, "/");
		strcat (path, d->d_name);
		if (stat (path, &filetab[nfile].st) < 0)
			continue;
		if ((filetab[nfile].st.st_mode & S_IFMT) == S_IFDIR)
			++ndir;
		else if ((filetab[nfile].st.st_mode & S_IFMT) != S_IFREG)
			continue;
		if (filetab[nfile].st.st_mtime > dmod)
			dmod = filetab[nfile].st.st_mtime;
	        filetab[nfile].path = strdup (path);
		filetab[nfile].name = strdup (d->d_name);
		if (! filetab[nfile].path || ! filetab[nfile].name)
			error (HS_InternalServerError,
				"no memory for file name");
		++nfile;
	}
	closedir (dd);

	qsort ((void*) filetab, nfile, sizeof (filerec), (int(*)())filecmp);

	sprintf (info, "%s/.info/%s", url.dirpath, url.basename);
	fd = fopen (info, "r");
	if (fd) {
		fgetinfo (info, sizeof (info), rinfo, sizeof (rinfo), 0, fd);
		iptr = (*rinfo && lang != L_ENG) ? rinfo : info;
		fclose (fd);
	} else
		iptr = "";

	mk_reply ();
	fprintf (reply, "<head><title>%s:%s", hostname,
		url.filepath + strlen (rootdir));
	if (*iptr) {
		canon (iptr);
		fprintf (reply, " - %s", iptr);
	}
	fprintf (reply, "</title>\r\n");
	fprintf (reply, "</head>\r\n");

	if (! putsection (reply, ".header.inc"))
		fprintf (reply, "<body>\r\n");

	if (*url.basename) {
		fprintf (reply, "<h1>%s", url.filepath + strlen (rootdir));
		if (*iptr)
			fprintf (reply, " - %s", iptr);
		fprintf (reply, "</h1>\r\n");
	} else
		fprintf (reply, "<h1>%s</h1>\r\n", hostname);

	sprintf (path, "%s/.index", url.filepath);
	if (access (path, 4) == 0) {
		fprintf (reply, "<form method=GET action=\"\">\r\n");
		fprintf (reply, "%s: <input name=search size=40>\r\n",
			lang == L_ENG ? "Enter search string" :
			"Введите шаблон для поиска");
		fprintf (reply, "<input type=submit value=\"%s\">\r\n",
			lang == L_ENG ? "Search" : "Поиск");
		fprintf (reply, "</form>\r\n");
	}

	strcpy (path, url.dirpath + strlen (rootdir));
	canon (path);
	escape (path);

	if (html & H_TABLES) {
		fprintf (reply, "<table cellspacing=0>\r\n");
		if (ndir == nfile) {
			fprintf (reply, "<tr><th>%s</th><th>%s</th></tr>\r\n",
				lang==L_ENG ? "Directory" : "Каталог",
				lang==L_ENG ? "Description" : "Описание");
			fprintf (reply, "<tr><td colspan=2><hr></td></tr>\r\n");
		} else {
			fprintf (reply, "<tr><th>%s</th><th>%s</th>"
				"<th>%s</th><th>%s</th></tr>\r\n",
				lang==L_ENG ? "File" : "Файл",
				lang==L_ENG ? "Bytes" : "Байты",
				lang==L_ENG ? "Date" : "Дата",
				lang==L_ENG ? "Description" : "Описание");
			fprintf (reply, "<tr><td colspan=4><hr></td></tr>\r\n");
		}
		fprintf (reply, "<tr><td colspan=4><img src=\"/icons/back.gif\" align=top>"
			"&nbsp;<a href=\"%s/\">%s</a></td></tr>\r\n",
			url.trailing_slash ? ".." : ".",
			lang==L_ENG ? "Back to the parent directory" :
			"Возврат в родительский каталог");
	} else
		fprintf (reply, "<pre><a href=\"%s/\">&lt;Up&gt;            </a>"
			"      &lt;dir&gt;           %s\r\n",
			url.trailing_slash ? ".." : ".",
			lang==L_ENG ? "Back to the parent directory" :
			"Возврат в родительский каталог");

	for (i=0; i<ndir; ++i) {
		strcpy (path, filetab[i].path + strlen (rootdir));
		canon (path);
		escape (path);
		strcpy (name, filetab[i].name);
		canon (name);

		sprintf (info, "%s/.info/%s", url.filepath, filetab[i].name);
		fd = fopen (info, "r");
		if (fd) {
			if (fstat (fileno (fd), &ist) >= 0 &&
			    ist.st_mtime > dmod)
				dmod = ist.st_mtime;
			fgetinfo (info, sizeof (info), rinfo, sizeof (rinfo), 0, fd);
			iptr = (*rinfo && lang != L_ENG) ? rinfo : info;
			fclose (fd);
			canon (iptr);
		} else
			iptr = "";

		if (html & H_TABLES) {
			fprintf (reply, "<tr valign=top><td><img src=\"/icons/dir.gif\" align=top>"
				"&nbsp;<a href=\"");
			if (! url.trailing_slash)
				fprintf (reply, "%s/", url.basename);
			fprintf (reply, "%s/\">%s</a>&nbsp;</td>\r\n",
				name, name);
			if (*iptr) {
				if (ndir != nfile)
					fprintf (reply, "<td></td><td></td>");
				fprintf (reply, "\t<td>%s</td>\r\n", iptr);
			}
			fprintf (reply, "</tr>\r\n");
		} else {
			fprintf (reply, "<a href=\"");
			if (! url.trailing_slash)
				fprintf (reply, "%s/", url.basename);
			fprintf (reply, "%s/\">", name);
			if (strlen (name) > 16)
				fprintf (reply, "%s\r\n\t\t", name);
			else
				fprintf (reply, "%-16s", name);
			fprintf (reply, "</a>      &lt;dir&gt;");
			if (*iptr) {
				fprintf (reply, "           ");
				printinfo (reply, iptr, 38, 38);
			} else
				fprintf (reply, "\r\n");
		}
	}

	for (; i<nfile; ++i) {
		strcpy (path, filetab[i].path + strlen (rootdir));
		canon (path);
		escape (path);
		strcpy (name, filetab[i].name);
		canon (name);

		sprintf (info, "%s/.info/%s", url.filepath, filetab[i].name);
		fd = fopen (info, "r");
		if (fd) {
			if (fstat (fileno (fd), &ist) >= 0 &&
			    ist.st_mtime > dmod)
				dmod = ist.st_mtime;
			fgetinfo (info, sizeof (info), rinfo, sizeof (rinfo), 0, fd);
			iptr = (*rinfo && lang != L_ENG) ? rinfo : info;
			fclose (fd);
			canon (iptr);
		} else
			iptr = "";

		if (html & H_TABLES) {
			fprintf (reply, "<tr valign=top><td><img src=\"/icons/%s.gif\" align=top>",
				icon (filetab[i].path));
			fprintf (reply, "&nbsp;<a href=\"");
			if (! url.trailing_slash)
				fprintf (reply, "%s/", url.basename);
			fprintf (reply, "%s\">%s</a>&nbsp;</td>\r\n",
				name, name);

			fprintf (reply, "\t<td align=right>&nbsp;");
		} else {
			fprintf (reply, "<a href=\"");
			if (! url.trailing_slash)
				fprintf (reply, "%s/", url.basename);
			fprintf (reply, "%s/\">", name);
			if (strlen (name) > 16)
				fprintf (reply, "%s\r\n\t\t", name);
			else
				fprintf (reply, "%-16s", name);
			fprintf (reply, "</a>");
		}

		if (filetab[i].st.st_size >= 1000000)
			fprintf (reply, "%3d,%03d,%03d",
				(int) (filetab[i].st.st_size / 1000000),
				(int) (filetab[i].st.st_size / 1000 % 1000),
				(int) (filetab[i].st.st_size % 1000));
		else if (filetab[i].st.st_size >= 1000)
			fprintf (reply, "    %3d,%03d",
				(int) (filetab[i].st.st_size / 1000),
				(int) (filetab[i].st.st_size % 1000));
		else
			fprintf (reply, "        %3d",
				(int) filetab[i].st.st_size);

		ptm = localtime (&filetab[i].st.st_mtime);
		mod = ptm->tm_year * 10000 + (ptm->tm_mon + 1) * 100 +
			ptm->tm_mday;

		if (html & H_TABLES) {
			fprintf (reply, "&nbsp;</td><td align=right>&nbsp;%d-%s-%02d&nbsp;</td>\r\n",
				(int) (mod % 100), month [mod / 100 % 100],
				(int) (mod / 10000));
			if (*iptr)
				fprintf (reply, "\t<td>%s</td>\r\n", iptr);
		} else {
			fprintf (reply, " %2d-%s-%02d", (int) (mod % 100),
				month [mod / 100 % 100], (int) (mod / 10000));
			if (*iptr) {
				fprintf (reply, " ");
				printinfo (reply, iptr, 38, 38);
			} else
				fprintf (reply, "\r\n");
		}
	}
	for (i=0; i<nfile; ++i) {
		free (filetab[i].path);
		free (filetab[i].name);
	}

	if (html & H_TABLES) {
		if (ndir == nfile)
			fprintf (reply, "<tr><td colspan=2><hr></td></tr>\r\n");
		else
			fprintf (reply, "<tr><td colspan=4><hr></td></tr>\r\n");
		fprintf (reply, "</table>\r\n");
	} else
		fprintf (reply, "</pre>\r\n");

	strcpy (path, url.filepath);
	strcat (path, "/README");
	fd = fopen (path, "r");
	if (! fd) {
		strcat (path, ".TXT");
		fd = fopen (path, "r");
	}
	if (fd) {
		if (fstat (fileno (fd), &ist) >= 0 && ist.st_mtime > dmod)
			dmod = ist.st_mtime;
		fprintf (reply, "<pre>\r\n");
		copy (fd, reply, 100*1024L, 0);
		fprintf (reply, "</pre>\r\n");
		fclose (fd);
	}
	putsection (reply, ".footer.inc");
	fprintf (reply, "</body>\r\n");

	if (dmod < langstamp)
		dmod = langstamp;
	if (dmod <= h_modstamp) {
		printf ("%s 304 Document not modified\r\n", HTTPVERSION);
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		if (keepalive)
			printf ("Connection: Keep-Alive\r\n");
		printf ("Content-Length: 0\r\n");
		printdate ("Date", now);
		printf ("\r\n");
		syslog (LOG_INFO, "[%s] directory %s not modified",
			peername, url.filepath);
		return;
	}

	if (proto != PROTO_0_9) {
		printf ("%s 200 Document follows\r\n", HTTPVERSION);
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		if (keepalive)
			printf ("Connection: Keep-Alive\r\n");
		printdate ("Date", now);
		ptm = gmtime (&dmod);
		strftime (dat, sizeof (dat), DATE822, ptm);
		printf ("Last-Modified: %s\r\n", dat);
		printdate ("Expires", now + 24*60*60L);
		printtype ("text/html", 1);
		if (method == M_HEAD) {
			printf ("Content-Length: %ld\r\n", ftell (reply));
			printf ("\r\n");
			syslog (LOG_INFO, "[%s] sent header of directory %s",
				peername, url.filepath + strlen(rootdir));
			return;
		}
	}

	put_reply ();
	syslog (LOG_INFO, "[%s] sent directory %s", peername,
		url.filepath + strlen(rootdir));
}

void sendmap ()
{
	char *ref;

	if (! map_read (url.filepath))
		error (HS_NotFound, "cannot read map `%s'", url.filepath);

	ref = map_find (url.search);
	if (! ref)
		error (HS_NotFound, "no location on map `%s' at `%s'",
			url.filepath, url.search);

	if (proto != PROTO_0_9) {
		printf ("%s 301 Document moved\r\n", HTTPVERSION);
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		if (keepalive)
			printf ("Connection: Keep-Alive\r\n");
		printdate ("Date", now);
	}

	printf ("Location: %s\r\n", ref);

	if (method == M_HEAD) {
		syslog (LOG_INFO, "[%s] sent map %s at %s header %s",
			peername, url.filepath + strlen(rootdir),
			url.search, ref);
		return;
	}

	mk_reply ();
	fprintf (reply, "%s\r\n", ref);
	fprintf (reply, "<p>This document is available from <a href=\"%s\">here</a>\r\n",
		ref);
	put_reply ();
	syslog (LOG_INFO, "[%s] sent map %s at %s reference %s",
		peername, url.filepath + strlen(rootdir), url.search, ref);
}

/*
 * Free request headers and body.
 */
void freereq ()
{
	int i;

	for (i=0; h_tab[i].name; ++i)
		if (*h_tab[i].value) {
			free (*h_tab[i].value);
			*h_tab[i].value = 0;
		}
	for (i=0; i<h_nxtab; ++i) {
		if (h_xtab[i].name)
			free (h_xtab[i].name);
		if (h_xtab[i].value)
			free (h_xtab[i].value);
	}
	h_nxtab = 0;
	if (h_user) {
		free (h_user);
		h_user = 0;
	}
	if (h_password) {
		free (h_password);
		h_password = 0;
	}
	if (h_body) {
		fclose (h_body);
		h_body = 0;
		h_blen = 0;
	}
}

char *decode64 (const char *bufcoded)
{
	int nprbytes, nbytesdecoded;
	const unsigned char *bufin;
	unsigned char *bufout;
	static char bufplain [256];
	static const unsigned char pr2six [256] = {
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,

		64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
		64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,

		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	};

	/* Strip leading whitespace. */
	while (*bufcoded == ' ' || *bufcoded == '\t')
		bufcoded++;

	/* Figure out how many characters are in the input buffer.
	 * Allocate this many from the per-transaction pool for the result. */
	bufin = (const unsigned char*) bufcoded;
	while (pr2six[*(bufin++)] <= 63)
		continue;
	nprbytes = (bufin - (const unsigned char*) bufcoded) - 1;
	nbytesdecoded = ((nprbytes + 3) / 4) * 3;
	if (nbytesdecoded >= sizeof (bufplain))
		return 0;

	bufout = (unsigned char*) bufplain;
	bufin = (const unsigned char*) bufcoded;
	while (nprbytes > 0) {
		*(bufout++) = pr2six[bufin[0]] << 2 | pr2six[bufin[1]] >> 4;
		*(bufout++) = pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2;
		*(bufout++) = pr2six[bufin[2]] << 6 | pr2six[bufin[3]];
		bufin += 4;
		nprbytes -= 4;
	}
	if (nprbytes & 03) {
		if (pr2six[bufin[-2]] > 63)
			nbytesdecoded -= 2;
		else
			nbytesdecoded -= 1;
	}
	bufplain[nbytesdecoded] = '\0';
	return bufplain;
}

/*
 * Read request headers and body, if present.
 */
void getreq (FILE *fd, int get_body_to_eof)
{
	char hline [LINESZ], *name, *value;
	int i, c;

	while (getstr (fd, hline, sizeof (hline))) {
		if (! *hline) {                 /* request body */
			h_blen = 0;
			if (! h_content_length && ! get_body_to_eof)
				break;          /* ignore body */
			h_body = ftemp ();
			if (! h_body)
				error (HS_InternalServerError,
					"cannot create temporary file");
			if (get_body_to_eof)
				while ((c = getc (fd)) >= 0) {
					putc (c, h_body);
					++h_blen;
				}
			else {
				int len = atoi (h_content_length);
				for (; h_blen<len; ++h_blen) {
					/* ignore EOF */
					c = (unsigned char) getc (fd);
					putc (c, h_body);
				}
			}
			break;
		}
		name = hline;
		for (value=hline; *value && *value!=':'; ++value);
		if (! *value)
			continue;               /* no colon found */
		*value++ = 0;
		while (*value==' ' || *value=='\t')
			++value;
		if (! *value)
			continue;               /* empty value */
		for (i=0; h_tab[i].name; ++i)
			if (strcasecmp (name, h_tab[i].name) == 0) {
				*h_tab[i].value = strdup (value);
				break;
			}
		if (! h_tab[i].name) {          /* extended header */
			for (i=0; i<h_nxtab; ++i)
				if (strcasecmp (name, h_xtab[i].name) == 0) {
					/* append value */
					char *old = *h_tab[i].value;
					if (! old) {
						h_xtab[i].value = strdup (value);
						break;
					}
					*h_tab[i].value = malloc (strlen (old) +
						strlen (value) + 3);
					if (*h_tab[i].value) {
						strcpy (*h_tab[i].value, old);
						strcat (*h_tab[i].value, ", ");
						strcat (*h_tab[i].value, value);
					}
					free (old);
					break;
				}
			if (i >= h_nxtab) {
				if (h_nxtab >= MAXH)
					continue;       /* too many headers */
				h_xtab[h_nxtab].name = strdup (name);
				h_xtab[h_nxtab].value = strdup (value);
				++h_nxtab;
			}
		}
	}
	if (h_if_modified_since)
		h_modstamp = getdate (h_if_modified_since);
	else
		h_modstamp = 0;
	if (h_authorization) {
		char *p = strrchr (h_authorization, ' ');
		if (p) {
			p = decode64 (p+1);
			if (p) {
				char *q = strchr (p, ':');
				if (q) {
					*q++ = 0;
					h_password = strdup (q);
				}
				h_user = strdup (p);
			}
		}
	}
}

/*
 * Print request headers and body trace information.
 */
void logreq ()
{
	int i;

	for (i=0; h_tab[i].name; ++i)
		if (h_tab[i].name && *h_tab[i].value)
			syslog (LOG_DEBUG, "[%s] request %s: %s",
				peername, h_tab[i].name, *h_tab[i].value);
	for (i=0; i<h_nxtab; ++i)
		if (h_xtab[i].name && h_xtab[i].value)
			syslog (LOG_DEBUG, "[%s] request extended %s: %s",
				peername, h_xtab[i].name, h_xtab[i].value);
	if (h_body)
		syslog (LOG_DEBUG, "[%s] request body %ld bytes",
			peername, h_blen);
}

#ifdef sun
#define setenv(name, value, overwrite) put_env (name, value)
#endif

void sendexec (char *arg)
{
	int pout[2], pin[2], pid, status;
	FILE *fout, *fin;
	struct xheader *x;
	char script[LINESZ], pathinfo[LINESZ], *p;

	while (*arg == '/')
		++arg;
	memset (script, 0, LINESZ);
	strcpy (script, rootdir);
	strcat (script, "/exec/");
	p = strchr (arg, '/');
	if (p) {
		strncat (script, arg, p-arg);
		strcpy (pathinfo, p);
	} else {
		strcat (script, arg);
		strcpy (pathinfo, "/");
	}

	/* check for existence of the file */
	if (stat (script, &filestat) < 0 ||
	    (filestat.st_mode & S_IFMT) != S_IFREG)
		error (HS_NotFound, "[%s] no such object `%s'",
			peername, url.locator);

	if (pipe (pout) < 0 || pipe (pin) < 0)
		error (HS_InternalServerError, "cannot create pipe");
	pid = debug < 2 ? vfork () : 0;
	if (pid < 0)
		error (HS_InternalServerError, "cannot create process");

	if (pid == 0) {
		char buf[LINESZ], *s;

		/* Child -- reopen pipes on stdin and stdout. */
		dup2 (2, 1);            /* stderr to stdout */
		dup2 (pin[0], 0);
		dup2 (pout[1], 1);
		close (pin[0]);
		close (pin[1]);
		close (pout[0]);
		close (pout[1]);

		/* Set environment variables. */
		setenv ("SERVER_PROTOCOL", HTTPVERSION, 1);
		setenv ("SERVER_SOFTWARE", VERSION, 1);
		setenv ("GATEWAY_INTERFACE", "CGI/1.1", 1);
		setenv ("REQUEST_METHOD", method_name[method], 1);

		setenv ("SERVER_NAME", hostname, 1);
		setenv ("SERVER_ADDR", inet_ntoa (my.sin_addr), 1);

		sprintf (buf, "%d", ntohs (my.sin_port));
		setenv ("SERVER_PORT", buf, 1);

		setenv ("REMOTE_ADDR", inet_ntoa (peer.sin_addr), 1);
		if (*peername<'0' || *peername>'9')
			setenv ("REMOTE_HOST", peername, 1);

		sprintf (buf, "%ld", h_blen);
		setenv ("CONTENT_LENGTH", buf, 1);

		if (h_content_type)
			setenv ("CONTENT_TYPE", h_content_type, 1);
		if (h_date)
			setenv ("HTTP_DATE", h_date, 1);
		if (h_mime_version)
			setenv ("HTTP_MIME_VERSION", h_mime_version, 1);
		if (h_pragma)
			setenv ("HTTP_PRAGMA", h_pragma, 1);
		if (h_from)
			setenv ("HTTP_FROM", h_from, 1);
		if (h_if_modified_since)
			setenv ("HTTP_IF_MODIFIED_SINCE", h_if_modified_since, 1);
		if (h_referer)
			setenv ("HTTP_REFERER", h_referer, 1);
		if (h_user_agent)
			setenv ("HTTP_USER_AGENT", h_user_agent, 1);
		if (h_allow)
			setenv ("HTTP_ALLOW", h_allow, 1);
		if (h_content_encoding)
			setenv ("HTTP_CONTENT_ENCODING", h_content_encoding, 1);
		if (h_expires)
			setenv ("HTTP_EXPIRES", h_expires, 1);
		if (h_last_modified)
			setenv ("HTTP_LAST_MODIFIED", h_last_modified, 1);
		if (h_connection)
			setenv ("HTTP_CONNECTION", h_connection, 1);

		for (x=h_xtab; x<h_xtab+h_nxtab; ++x) {
			/* Translate names to uppercase,
			 * replace mimus by underscore */
			sprintf (buf, "HTTP_%s", x->name);
			for (p=buf; *p; ++p)
				if (*p == '-')
					*p = '_';
				else if (*p>='a' && *p<='z')
					*p += 'A' - 'a';
			setenv (buf, x->value, 1);
		}

		/* Authentication is not supported yet. */
		if (h_authorization) {
			char *p = strchr (h_authorization, ' ');
			if (p)
				*p = 0;
			setenv ("AUTH_TYPE", h_authorization, 1);
			if (p)
				*p = ' ';
			/* Too insecure:
			setenv ("HTTP_AUTHORIZATION", h_authorization, 1); */
		}
		if (h_user)
			setenv ("REMOTE_USER", h_user, 1);
		if (*url.search) {
			strcpy (buf, url.search);
			escape (buf);
			setenv ("QUERY_STRING", buf, 1);
		}

		setenv ("SCRIPT_NAME", script + strlen (rootdir), 1);
		setenv ("PATH_INFO", pathinfo, 1);
		/* PATH_TRANSLATED not supported. */

		strcpy (buf, script);
		s = strrchr (buf, '/');         /* always nonzero */
		*s++ = 0;
		chdir (buf);
		setenv ("HOME", buf, 1);

		_exit (execle (script, s, 0, environ));
	}
	close (pin[0]);
	close (pout[1]);

	/* Parent -- make FILE's from pipes. */
	fin = fdopen (pout[0], "r");
	if (! fin)
		error (HS_InternalServerError, "cannot reopen input pipe");

	/* Put body. */
	fout = fdopen (pin[1], "w");
	if (! fout)
		error (HS_InternalServerError, "cannot reopen output pipe");
	if (h_blen)
		copy (h_body, fout, h_blen, -translate);
	fclose (fout);
	close (pin[1]);

	if (filestat.st_mode & (S_ISUID | S_ISGID)) {
		for (;;) {
			int c = getc (fin);
			if (c < 0)
				break;
			putc (c, stdout);
		}
		fclose (fin);
		close (pout[0]);
		goto done;
	}

	/* Get reply. */
	freereq ();
	getreq (fin, 1);
	fclose (fin);
	close (pout[0]);

	/* Get child status. */
	if (waitpid (pid, &status, 0) < 0)
		error (HS_InternalServerError, "error waiting for child");
	if (! WIFEXITED (status))
		error (HS_InternalServerError, "child aborted");
	if (WEXITSTATUS (status) != 0)
		error (HS_InternalServerError, "child failed with status %d",
			WEXITSTATUS (status));

	/* Put reply. */
	if (proto != PROTO_0_9) {
		printf ("%s %s\r\n", HTTPVERSION, h_status ? h_status :
			"200 Document follows");
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		if (h_connection)
			printf ("Connection: %s\r\n", h_connection);
		printdate ("Date", now);
		printf ("Content-Length: %lu\r\n", (unsigned long) h_blen);
		if (h_blen && h_content_type)
			printtype (h_content_type, 1);
		if (h_blen && h_content_encoding)
			printf ("Content-Encoding: %s\r\n", h_content_encoding);
		if (h_location)
			printf ("Location: %s\r\n", h_location);
		if (h_mime_version)
			printf ("MIME-Version: %s\r\n", h_mime_version);
		if (h_pragma)
			printf ("Pragma: %s\r\n", h_pragma);
		if (h_www_authenticate)
			printf ("WWW-Authenticate: %s\r\n", h_www_authenticate);
		if (h_allow)
			printf ("Allow: %s\r\n", h_allow);
		if (h_expires)
			printf ("Expires: %s\r\n", h_expires);
		if (h_last_modified)
			printf ("Last-Modified: %s\r\n", h_last_modified);

		for (x=h_xtab; x<h_xtab+h_nxtab; ++x)
			printf ("%s: %s\r\n", x->name, x->value);
		printf ("\r\n");

		if (method == M_HEAD) {
			syslog (LOG_INFO, "[%s] sent header exec %s",
				peername, url.filepath + strlen(rootdir));
			return;
		}
	}
	if (h_blen)
		copy (h_body, stdout, h_blen, translate);
done:
	syslog (LOG_INFO, "[%s] sent exec %s total %d bytes",
		peername, url.filepath + strlen(rootdir), h_blen);
}

/*
 * Add a new antry to multihome list.
 * Argument is IPADDR:PATH.
 */
void addhome (char *arg)
{
	unsigned long ipaddr;
	struct hometab *h;

	ipaddr = inet_addr (arg);
	if (ipaddr == INADDR_NONE)
		return;
	arg = strchr (arg, ':');
	if (! arg)
		return;

	if (homelist.homedir) {
		h = (struct hometab*) malloc (sizeof (struct hometab));
		if (! h)
			return;
		*h = homelist;
		homelist.next = h;
	}
	homelist.homedir = arg + 1;
	homelist.ipaddr = ntohl (ipaddr);
}

/*
 * Find an entry in multihome list.
 * Return the path.  Never return 0.
 * Set the hostname.
 */
char *findhome ()
{
	struct hometab *h;
	struct hostent *e;
	unsigned long ipaddr = ntohl (my.sin_addr.s_addr);

	for (h= &homelist; h; h=h->next)
		if (h->ipaddr == ipaddr)
			break;
	if (! h)
		return (ROOTDIR);

	if (! h->hostname) {
		e = gethostbyaddr ((char*)&my.sin_addr.s_addr, 4, AF_INET);
		if (e)
			h->hostname = strdup (e->h_name);
	}
	if (h->hostname)
		strcpy (hostname, h->hostname);
	else
		strcpy (hostname, inet_ntoa (my.sin_addr));
	return (h->homedir);
}

/*
 * Load the authentication table.
 * Format:
 *      /what [ realm ]
 * Default realm is "common".
 * Comments should begin with '#'.
 */
void loadauthtab (FILE *fd)
{
	char *p, *what, *user, *password, *realm;
	struct auth *a;

	while (fgets (line, sizeof(line), fd)) {
		/* Strip comment. */
		p = strchr (line, '#');
		if (p) *p = 0;

		/* Strip trailing spaces. */
		p = line + strlen (line) - 1;
		while (p>=line && (*p == '\r' || *p == '\n' ||
		    *p == '\t' || *p == ' '))
			*p-- = 0;

		/* Skip leading spaces. */
		p = line;
		while (*p==' ' || *p=='\t') ++p;
		if (! *p) continue;

		/* Get directory name. */
		what = p;
		while (*p && *p!=' ' && *p!='\t') ++p;
		if (*p) {
			*p++ = 0;
			while (*p==' ' || *p=='\t') ++p;
		}

		/* Get user name. */
		user = p;
		while (*p && *p!=' ' && *p!='\t') ++p;
		if (*p) {
			*p++ = 0;
			while (*p==' ' || *p=='\t') ++p;
		}

		/* Get password. */
		password = p;
		while (*p && *p!=' ' && *p!='\t') ++p;
		if (*p) {
			*p++ = 0;
			while (*p==' ' || *p=='\t') ++p;
		}

		/* Get realm. */
		if (! *p)
			realm = "common";
		else
			realm = p;

		/* Add new entry to auth list. */
		a = (struct auth*) malloc (sizeof (struct auth));
		if (! a)
nomem:                  error (HS_InternalServerError,
				"no memory for auth list entry");
		a->dir = strdup (what);
		a->user = strdup (user);
		a->password = strdup (password);
		a->realm = strdup (realm);
		if (! a->dir || ! a->realm)
			goto nomem;
		a->dirlen = strlen (a->dir);
		a->next = authlist;
		authlist = a;
	}
}

/*
 * Load the forward table.
 * Format:
 *      /what host[:port] /where [ user password ["public"] [] ]
 * Default port is 80.
 * Comments should begin with '#'.
 * Invalid lines are silently ignored.
 */
void loadfwtab (FILE *fd)
{
	char *p, *what, *host, *dest, *user, *passwd;
	struct forward *a;
	int port, flags;

	while (fgets (line, sizeof(line), fd)) {
		/* Strip comment. */
		p = strchr (line, '#');
		if (p) *p = 0;

		/* Strip trailing spaces. */
		p = line + strlen (line) - 1;
		while (p>=line && (*p == '\r' || *p == '\n' ||
		    *p == '\t' || *p == ' '))
			*p-- = 0;

		/* Skip leading spaces. */
		p = line;
		while (*p==' ' || *p=='\t') ++p;
		if (! *p) continue;

		/* Get directory name. */
		what = p;
		while (*p && *p!=' ' && *p!='\t') ++p;
		if (*p) *p++ = 0;
		while (*p==' ' || *p=='\t') ++p;
		if (! *p) continue;

		/* Get host. */
		host = p;
		port = 80;
		while (*p && *p!=' ' && *p!='\t')
			if (*p++ == ':') {
				port = atoi (p);
				p[-1] = 0;
			}
		if (*p) *p++ = 0;
		while (*p==' ' || *p=='\t') ++p;
		if (! *p) continue;

		/* Get destination path. */
		dest = p;
		while (*p && *p!=' ' && *p!='\t') ++p;
		if (*p) *p++ = 0;
		while (*p==' ' || *p=='\t') ++p;

		/* Get user name and encoded password string. */
		flags = 0;
		if (! *p)
			user = passwd = 0;
		else {
			/* Get user name. */
			user = p;
			while (*p && *p!=' ' && *p!='\t') ++p;
			if (*p) *p++ = 0;
			while (*p==' ' || *p=='\t') ++p;
			if (! *p) continue;

			/* Get user name. */
			passwd = p;
			while (*p && *p!=' ' && *p!='\t') ++p;
			if (*p) *p++ = 0;
			while (*p==' ' || *p=='\t') ++p;

			if (*p && strcasecmp (p, "public") == 0)
				flags |= FW_PUBLIC;

		}

		/* Add new entry to forward list. */
		a = (struct forward*) malloc (sizeof (struct forward));
		if (! a)
nomem:                  error (HS_InternalServerError,
				"no memory for forward list entry");
		a->dir = strdup (what);
		a->host = strdup (host);
		a->dest = strdup (dest);
		if (! a->dir || ! a->host || ! a->dest)
			goto nomem;
		a->dirlen = strlen (a->dir);
		a->user = user ? strdup (user) : 0;
		a->password = passwd ? strdup (passwd) : 0;
		a->flags = flags;
		a->ipaddr = 0;          /* resolved later */
		a->port = port;
		a->sockfd = 0;
		a->next = forwlist;
		forwlist = a;
	}
}

struct forward *findforward (char *path)
{
	struct forward *f;

	for (f=forwlist; f; f=f->next)
		if (strncmp (path, f->dir, f->dirlen) == 0 &&
		    (path[f->dirlen] == 0 || path[f->dirlen] == '/'))
			return (f);
	return (0);
}

char *auth_ok (char *path, char *user, char *password)
{
	struct auth *f;
	int level = -1, l;
	char *realm = 0;

	if (! user)
		password = 0;
	for (f=authlist; f; f=f->next) {
		/* Ищем имя каталога, если не '*'. */
		if (f->dir[0] == '*')
			l = 0;
		else if (strncmp (path, f->dir, f->dirlen) == 0 &&
		    (path[f->dirlen] == 0 || path[f->dirlen] == '/'))
			l = f->dirlen;
		else
			continue;
		/* Требуется имя и пароль. */
		if (! user)
			return f->realm;
		/* Длинное имя приоритетнее. */
		if (l < level)
			continue;
		/* Считаем, что доступ закрыт. */
		if (l > level)
			realm = f->realm;
		level = l;
		/* Сравниваем имя пользователя, если не '*'. */
		if (f->user[0] != '*' && strcmp (user, f->user) != 0)
			continue;
		/* Нельзя, если пароль равен '-'. */
		if (f->password[0] == '-')
			continue;
		/* Сравниваем пароль, если не '*'. */
		if (f->password[0] != '*' && strcmp (password, f->password) != 0)
			continue;
		realm = 0;
	}
	/* Каталог не найден - значит, можно. */
	if (level < 0)
		return 0;
	return realm;
}

void check_auth ()
{
	char *realm;

	if (! authlist)
		return;
	realm = auth_ok (url.reqname, h_user, h_password);
	if (! realm)
		return;

	printf ("%s 401 Unauthorized\r\n", HTTPVERSION);
	printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
	printdate ("Date", now);
	printf ("WWW-Authenticate: BASIC realm=\"%s\"\r\n\r\n", realm);

	syslog (LOG_INFO, "[%s] sent auth request for %s",
		peername, url.filepath + strlen(rootdir));
	fflush (stdout);
	exit (-1);
}

void sendforward (struct forward *f)
{
	char buf [LINESZ], *req;
	struct xheader *x;
	int status;

#if 0
	/* Check authentication. */
	if (! (f->flags & FW_PUBLIC)) {
		f->user
		f->password
		???
	}
#endif
	/* Resolve host name. */
	if (! f->ipaddr) {
		struct hostent *hp;
		struct hostent hbuf;
		struct in_addr hbufaddr;

		if (*f->host>='0' && *f->host<='9' &&
		    (hbufaddr.s_addr = inet_addr (f->host)) != INADDR_NONE) {
			/* raw ip address */
			hp = &hbuf;
#ifdef h_addr
			{
			static char *alist [1];
			hp->h_addr_list = alist;
			}
#endif
			hp->h_name = f->host;
			hp->h_addrtype = AF_INET;
			hp->h_addr = (void*) &hbufaddr;
			hp->h_length = sizeof (struct in_addr);
			hp->h_aliases = 0;
		} else {
			hp = gethostbyname (f->host);
			if (! hp)
				error (HS_InternalServerError,
					"cannot resolve host name");
		}
		f->ipaddr = ntohl (((struct in_addr*)hp->h_addr)->s_addr);
	}

	/* Connect to the server. */
	if (! f->sockfd) {
		int sock = socket (AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in server;

		if (sock < 0)
			error (HS_InternalServerError,
				"cannot create socket");

		/* Connect to the destination server. */
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = htonl (f->ipaddr);
		server.sin_port = htons (f->port);

		if (connect (sock, (struct sockaddr*) &server,
		    sizeof (server)) < 0)
			error (HS_ServiceUnavailable,
				"Cannot connect to %s:%d, try again later",
				f->host, f->port);
		f->sockfd = fdopen (sock, "r+");
	}

	/* Make new URL. */
	strcpy (buf, f->dest);
	req = url.reqname + f->dirlen;
	if (buf[strlen(buf)-1] == '/')
		while (*req == '/')
			++req;
	if (*req)
		strcat (buf, req);
	if (url.trailing_slash)
		strcat (buf, "/");
	if (*url.search) {
		strcat (buf, "?");
		strcat (buf, url.search);
	}
	unescape (buf);

	/* Send request. */
	fprintf (f->sockfd, "%s %s %s\r\n", method_name[method], buf,
		HTTPVERSION);
	fprintf (f->sockfd, "Connection: Keep-Alive\r\n");
	fprintf (f->sockfd, "Content-Length: %ld\r\n", h_blen);
#define PUT(var,name) if (var) fprintf (f->sockfd, name ": %s\r\n", var)
	PUT (h_content_type,      "Content-Type");
	PUT (h_content_encoding,  "Content-Encoding");
	PUT (h_mime_version,      "MIME-Version");
	PUT (h_last_modified,     "Last-Modified");
	PUT (h_expires,           "Expires");
	PUT (h_date,              "Date");
	PUT (h_from,              "From");
	PUT (h_if_modified_since, "If-Modified-Since");
	PUT (h_referer,           "Referer");
	PUT (h_user_agent,        "User-Agent");
	PUT (h_allow,             "Allow");
	PUT (h_pragma,            "Pragma");
#undef PUT
	for (x=h_xtab; x<h_xtab+h_nxtab; ++x)
		fprintf (f->sockfd, "%s: %s\r\n", x->name, x->value);
	fprintf (f->sockfd, "\r\n");

	/* Put request body. */
	if (h_blen)
		copy (h_body, f->sockfd, h_blen, 0);
	fflush (f->sockfd);

	/* Get reply. */
	if (! getstr (f->sockfd, buf, sizeof (buf)))
		error (HS_InternalServerError, "No reply from %s:%d",
			f->host, f->port);
	if (strncmp (buf, HTTPVERSION " ", sizeof (HTTPVERSION)) != 0)
		error (HS_InternalServerError, "Invalid reply from %s:%d - %s",
			buf);
	status = atoi (buf + sizeof (HTTPVERSION));
	freereq ();
	getreq (f->sockfd, 0);

	if (status/100 == 5 || ! h_connection ||
	    strcasecmp (h_connection, "keep-alive") != 0) {
		fclose (f->sockfd);
		f->sockfd = 0;
	}

	/* Put reply. */
	if (proto != PROTO_0_9) {
		printf ("%s\r\n", buf);
		printf ("Server: %s (%s)\r\n", VERSION, COPYRIGHT);
		if (keepalive)
			printf ("Connection: Keep-Alive\r\n");
		printdate ("Date", now);
		printf ("Content-Length: %lu\r\n", (unsigned long) h_blen);
		if (h_blen && h_content_type)
			printtype (h_content_type, 1);
		if (h_blen && h_content_encoding)
			printf ("Content-Encoding: %s\r\n", h_content_encoding);
		if (h_location)
			printf ("Location: %s\r\n", h_location);
		if (h_mime_version)
			printf ("MIME-Version: %s\r\n", h_mime_version);
		if (h_pragma)
			printf ("Pragma: %s\r\n", h_pragma);
		if (h_www_authenticate)
			printf ("WWW-Authenticate: %s\r\n", h_www_authenticate);
		if (h_allow)
			printf ("Allow: %s\r\n", h_allow);
		if (h_expires)
			printf ("Expires: %s\r\n", h_expires);
		if (h_last_modified)
			printf ("Last-Modified: %s\r\n", h_last_modified);

		for (x=h_xtab; x<h_xtab+h_nxtab; ++x)
			printf ("%s: %s\r\n", x->name, x->value);
		printf ("\r\n");

		if (method == M_HEAD) {
			syslog (LOG_INFO, "[%s] sent header forward %s status %d",
				peername, url.filepath + strlen(rootdir),
				status);
			return;
		}
	}
	if (h_blen)
		copy (h_body, stdout, h_blen, translate);

	syslog (LOG_INFO, "[%s] sent forward %s total %d bytes, status %d",
		peername, url.filepath + strlen(rootdir), h_blen, status);
}

void main (int argc, char **argv)
{
	int addrlen, keepopt;
	struct hostent *h;
	struct linger linger;
	struct forward *f;
	char *arg;

	progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "vDrd:l:")) {
		case EOF:
			break;
		case 'v':
			++verbose;
			continue;
		case 'D':
			++debug;
			continue;
		case 'r':
			++rusflag;
			continue;
		case 'd':
			if (strchr (optarg, ':'))
				addhome (optarg);
			else
				rootdir = optarg;
			continue;
		case 'l':
			if (*optarg == 0)
				lang = L_ENG;           /* english language */
			else if (strcasecmp (optarg, "unix") == 0)
				lang = L_KOI8;          /* russian koi8-r */
			else if (strcasecmp (optarg, "win") == 0)
				lang = L_WIN;           /* russian ms windows */
			else if (strcasecmp (optarg, "dos") == 0)
				lang = L_DOS;           /* russian ms dos */
			else if (strcasecmp (optarg, "mac") == 0)
				lang = L_MAC;           /* russian macintosh */
			else
				lang = L_ENG;           /* english by default */
			continue;
		}
		break;
	}
	argc -= optind;
	argv += optind;

	/* On exit, wait 5 minutes for output to drain. */
	linger.l_onoff = 1;
	linger.l_linger = 5*60;
	setsockopt (0, SOL_SOCKET, SO_LINGER, &linger, sizeof (linger));

	keepopt = 1;
	setsockopt (0, SOL_SOCKET, SO_KEEPALIVE, &keepopt, sizeof (keepopt));

	openlog ("liteweb", LOG_PID, LOG_DAEMON);

	if (argc != 0)
		error (HS_InternalServerError, "invalid argument `%s'",
			argv[0]);

	if (gethostname (hostname, sizeof (hostname)) < 0)
		error (HS_InternalServerError,
			"cannot determine my host name: %s", strerror (errno));

	memset (&my, 0, sizeof (my));
	memset (&peer, 0, sizeof (peer));
	if (! debug) {
		addrlen = sizeof (my);
		if (getsockname (0, (struct sockaddr *) &my, &addrlen) < 0)
			error (HS_InternalServerError,
				"cannot determine my address: %s",
				strerror (errno));

		if (! rootdir)
			rootdir = findhome ();

		addrlen = sizeof (peer);
		if (getpeername (0, (struct sockaddr *) &peer, &addrlen) < 0)
			error (HS_InternalServerError,
				"cannot determine peer address: %s",
				strerror (errno));

		h = gethostbyaddr ((char *) &peer.sin_addr, sizeof (peer.sin_addr), AF_INET);
		if (h)
			strcpy (peername, h->h_name);
		else
			strcpy (peername, inet_ntoa (peer.sin_addr));
		syslog (LOG_INFO, "[%s] connection from %s port %d",
			peername, inet_ntoa (peer.sin_addr),
			ntohs (my.sin_port));
#ifdef sun
		if (my.sin_addr.s_addr != 0x90ce880a)
			exit (0);
#endif
	} else if (! rootdir)
		rootdir = ROOTDIR;

	signal (SIGINT, interrupt);
	signal (SIGQUIT, interrupt);
	signal (SIGTERM, interrupt);

	/*
	 * Load the forward table.
	 */
	{
	FILE *fd;

	strcpy (line, rootdir);
	strcat (line, "/.forward");
	fd = fopen (line, "r");
	if (fd) {
		loadfwtab (fd);
		fclose (fd);
	}
	}

	/*
	 * Load the authentication table.
	 */
	{
	FILE *fd;

	strcpy (line, rootdir);
	strcat (line, "/.auth");
	fd = fopen (line, "r");
	if (fd) {
		loadauthtab (fd);
		fclose (fd);
	}
	}

again:
	time (&now);
	++reqcnt;

	/* Break a connection when the client goes sleeping. */
	alarm (TIMEOUT);
	if (! getstr (stdin, line, sizeof (line)))
		/* Broken connection - no need to log it. */
		exit (0);
	alarm (0);

	inputptr = line;
	arg = getarg ();
	if (! arg)
		goto again;             /* empty command */

	if (strcasecmp (arg, "GET") == 0)
		method = M_GET;
	else if (strcasecmp (arg, "HEAD") == 0)
		method = M_HEAD;
	else if (strcasecmp (arg, "POST") == 0)
		method = M_POST;
	else
		error (HS_BadRequest, "invalid command `%s'", arg);

	arg = getarg ();
	if (! arg)
		error (HS_BadRequest, "missing command argument `%s'", arg);
	strcpy (url.locator, arg);

	setproctitle ("%s + %s", peername, url.locator);

	arg = getarg ();
	if (! arg)
		proto = PROTO_0_9;
	else if (strcmp (arg, "HTTP/1.1") == 0)
		proto = PROTO_1_1;
	else
		proto = PROTO_1_0;

	if (proto != PROTO_0_9) {
		/* get request headers and body */
		getreq (stdin, 0);
		keepalive = h_connection &&
			strcasecmp (h_connection, "keep-alive") == 0;
		if (verbose) {
			syslog (LOG_INFO, "[%s] #%d %s%d %s%s%s",
				peername, reqcnt, method_name[method],
				proto, url.locator,
				h_if_modified_since ? " if modified" : "",
				keepalive ? ", keepalive" : "");
			if (h_from)
				syslog (LOG_INFO, "[%s] from %s",
					peername, h_from);
			if (h_user_agent)
				syslog (LOG_INFO, "[%s] user agent %s",
					peername, h_user_agent);
			if (h_referer)
				syslog (LOG_INFO, "[%s] referer %s",
					peername, h_referer);
			if (h_blen)
				syslog (LOG_INFO, "[%s] body %d bytes",
					peername, h_blen);
		}
		if (verbose > 1)
			logreq ();
	} else if (verbose)
		syslog (LOG_INFO, "[%s] GET0.9 %s%s",
			peername, url.locator);

	/*
	 * Compute processing modes:
	 * Language: English | Russian KOI-8r | Russian Windows | Russian MS-DOS
	 * HTML mode: Text | Graphics, Tables
	 */
	if (! html) {
		if (! h_user_agent ||
		    strcasestr (h_user_agent, "linemode") ||
		    strcasestr (h_user_agent, "lynx"))
			html |= H_TEXT;
		else {
			html |= H_GRAPH;
			if (strcasestr (h_user_agent, "mozilla"))
				html |= H_TABLES;
		}
	}
	if (! lang && h_user_agent) {
		char agent[32], *p;

		/* The language mode is dependent on
		 * the user agent software. */
		strncpy (agent, h_user_agent, sizeof(agent));
		agent[sizeof(agent)-1] = 0;

		/* Get the agent name */
		p = strchr (agent, ' ');
		if (p)
			*p = 0;

		/* Remove the version */
		p = strchr (agent, '/');
		if (p)
			*p = 0;

		/* Microsoft explorer says it is Mozilla -- megalomania? */
		if (strcasestr (h_user_agent, "msie"))
			strcpy (agent, "Microsoft");

		if (strcasestr (h_user_agent, "x11") ||
		    strcasestr (h_user_agent, "x window"))
			os = OS_X11;
		else if (strcasestr (h_user_agent, "macintosh"))
			os = OS_MAC;
		else if (strcasestr (h_user_agent, "winnt"))
			os = OS_MSWINNT;
		else if (strcasestr (h_user_agent, "windows 95") ||
		    strcasestr (h_user_agent, "win95"))
			os = OS_MSWIN95;
		else if (strcasestr (h_user_agent, "windows") ||
		    strcasestr (h_user_agent, "win16") ||
		    strcasestr (h_user_agent, "win32"))
			os = OS_MSWIN;
		else
			os = OS_UNKNOWN;

		lang = getuser (ntohl (my.sin_addr.s_addr),
			ntohl (peer.sin_addr.s_addr), agent, os, &langstamp);
		if (debug && lang)
			syslog (LOG_INFO, "[%s] lang %d stamp 0x%lx",
				peername, lang, langstamp);
	}
	if (! lang && rusflag) {
		/* For domains .su/.ru/.ua/.by use rusian
		 * language by default. */
		char *domain = strrchr (peername, '.');
		if (domain && (strcmp (domain, ".su") == 0 ||
		    strcmp (domain, ".ru") == 0 ||
		    strcmp (domain, ".ua") == 0 ||
		    strcmp (domain, ".by") == 0))
			switch (os) {
			default:         lang = L_KOI8; break;
			case OS_MAC:     lang = L_MAC;  break;
			case OS_MSWINNT:
			case OS_MSWIN95:
			case OS_MSWIN:   lang = L_WIN;  break;
			}
	}
	if (! lang)
		lang = L_ENG;

	/*
	 * Set up the coding translation table,
	 * based on the language selected.
	 */
	translate = 1;
	switch (lang) {
	default:    translate = 0;                       break;
	case L_WIN: memcpy (touser+128, trans_win, 128); break;
	case L_DOS: memcpy (touser+128, trans_dos, 128); break;
	case L_MAC: memcpy (touser+128, trans_mac, 128); break;
	}
	if (translate) {
		int i;

		for (i=0; i<128; ++i)
			touser[i] = fromuser[i] = i;
		memset (fromuser+128, '?', 128);
		for (i=0; i<256; ++i)
			fromuser[touser[i]] = i;
	}

	/* suppress file names beginning with period */
	parse ();
	if (strstr (url.filepath, "/."))
		error (HS_Forbidden, "incorrect file path `%s'", url.locator);

	check_auth ();

	if (strncmp (url.reqname, "/exec/", 6) == 0) {
		sendexec (url.reqname + 6);
	} else if ((f = findforward (url.reqname))) {
		sendforward (f);
	} else {
		/* check for existence of the file */
		if (stat (url.filepath, &filestat) < 0)
			error (HS_NotFound, "no such object `%s'", url.locator);

		if ((filestat.st_mode & S_IFMT) == S_IFDIR) {
			senddir ();
		} else if ((filestat.st_mode & S_IFMT) != S_IFREG)
			error (HS_NotFound, "no such object `%s'", url.locator);
		else if (strcmp (url.ext, ".map") == 0) {
			sendmap ();
		} else {
			send_file ();
		}
	}

	fflush (stdout);
	if (keepalive) {
		freereq ();
		setproctitle ("%s - %s", peername, url.locator);
		goto again;
	}

	exit (0);
}

/*
 * Get multistring from the file.
 * The continuation lines are indented by spaces.
 */
int fgetinfo (char *info, int maxlen, char *rinfo, int rmaxlen,
	long *date, FILE *fd)
{
	char *p;
	int n;

	*info = *rinfo = 0;
	if (date)
		*date = 0;
again:
	do n = getc (fd);
	while (n == ' ' || n == '\t');
	if (n < 0)
		return (0);
	ungetc (n, fd);

	fgets (info, maxlen, fd);
	if (ISDIGIT (info[0]) && ISDIGIT (info[1]) &&
	    ISDIGIT (info[2]) && ISDIGIT (info[3]) &&
	    ISDIGIT (info[4]) && ISDIGIT (info[5])) {
		char *q = info;

		if (date)
			*date = atol (info);
		for (p=info+6; *p==' ' || *p=='\t'; ++p)
			continue;
		while (*p)
			*q++ = *p++;
		*q = 0;
	}
	p = info;
	while (maxlen > 0) {
		n = strlen (p);
		maxlen -= n;
		p += n;

		while (--n >= 0 && (p[-1] == '\n' || p[-1] == '\15')) {
			*--p = 0;
			++maxlen;
		}

		n = getc (fd);
		if (n < 0)
			break;
		if (n != ' ' && n != '\t') {
			char buf[9];

			ungetc (n, fd);
			if (n != 'R' || ! fgets (buf, 9, fd) ||
			    strncmp (buf, "Russian:", 8) != 0)
				break;
			info = rinfo;
			maxlen = rmaxlen;
			goto again;
		}

		do n = getc (fd);
		while (n == ' ' || n == '\t');
		if (n < 0)
			break;
		ungetc (n, fd);
		*p++ = ' ';
		--maxlen;

		fgets (p, maxlen, fd);
	}
	return (1);
}
