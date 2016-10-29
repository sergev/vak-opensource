/*
 * Remote login daemon, based on UDP protocol.
 *
 * Copyright (C) 1994 Cronyx Ltd.
 * Authors: Serge Vakulenko, <vak@cronyx.msk.su>
 *          Sergey Ryzhkov, <sir@cronyx.msk.su>
 * Date: Wed Jul 27 21:27:58 MSD 1994
 */
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <signal.h>
#include <paths.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include "dlp.h"

#define log1(m)            { sprintf(logmsg,m); writelog(); }
#define log2(m,a)	   { sprintf(logmsg,m,a); writelog(); }
#define log3(m,a,b)	   { sprintf(logmsg,m,a,b); writelog(); }
#define log7(m,a,b,c,d,e,f){ sprintf(logmsg,m,a,b,c,d,e,f); writelog(); }

#define PTYNAME         "/dev/ptyXX"
#define PTYLEN          sizeof("/dev/ptyXX")
#define MTU             (2*1024)        /* maximum packet size */
#define LINESZ          512             /* maximum input line length */

char buf[MTU];
char logmsg[LINESZ];
char peername[70] = "<unknown>";
int port;
char *logfile;
int verbose;
int debug;
struct sockaddr_in peerfrom;
unsigned long ibytes, obytes;
unsigned long ipackets, opackets;
time_t t0;

extern char *optarg;
extern int optind;
extern int errno;

/*
 * Convert the Unix time value to the text format.
 * Return the pointer to the internal static buffer.
 * The time format is: dd.mm.yy/hh:mm:ss
 */
char *time2a (long tim)
{
	struct tm *ptm;
	static char cdate [30];

	ptm = localtime (&tim);
	sprintf (cdate, "%02d.%02d.%02d/%02d:%02d:%02d",
		ptm->tm_mday, ptm->tm_mon+1, ptm->tm_year,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return (cdate);
}

void writelog ()
{
	FILE *fd = logfile ? fopen (logfile, "a") : 0;
	fprintf (fd ? fd : stderr, "%s  %s  %d  %s\n",
		time2a (time ((long *) 0)), peername, port, logmsg);
	if (fd)
		fclose (fd);
}

void quit (int err)
{
	if (err >= -1) {
		buf[0] = DLP_LOGOUT;
		sendto (0, buf, 1, 0, (struct sockaddr*) &peerfrom, sizeof peerfrom);
	}
	log7 ("%s total %lu seconds %lu ibytes %lu obytes %lu ipackets %lu opackets",
		err ? "abort" : "logout", time (0) - t0,
		ibytes, obytes, ipackets, opackets);
	exit (err);
}

int forklogin (char *peerhost)
{
	static char line[] = PTYNAME;
	int i, c, master, slave;
	struct termios tt;
	struct stat st;

	master = -1;
	line[PTYLEN-6] = 'p';
	for (c='p'; master<0 && c<='s'; ++c) {
		line[PTYLEN-3] = c;
		line[PTYLEN-2] = '0';
		if (stat (line, &st) < 0)
			break;
		for (i=0; master<0 && i<16; ++i) {
			line[PTYLEN-2] = "0123456789abcdef"[i];
			master = open (line, 2);
		}
	}
	if (master < 0) {
		log1 ("out of ptys");
		return (-1);
	}
	line[PTYLEN-6] = 't';
	slave = open (line, 2);         /* Slave is now slave side */
	if (slave < 0) {
		log2 ("cannot open slave pty %s", line);
		return (-1);
	}

	i = fork ();
	if (i < 0) {
		log1 ("cannot fork");
		return (-1);
	}
	if (! i) {
		/* Child */
		close (master);
		setsid ();
		ioctl (slave, TIOCSCTTY, (char*)0);
		dup2 (slave, 0);
		dup2 (slave, 1);
		dup2 (slave, 2);
		if (slave > 2)
			close (slave);

		/* Set slave tty modes */
		tcgetattr (0, &tt);
		tt.c_iflag = TTYDEF_IFLAG;
		tt.c_oflag = TTYDEF_OFLAG;
		tt.c_lflag = TTYDEF_LFLAG;
		tcsetattr (0, TCSAFLUSH, &tt);

		execl ("/usr/bin/login", "login", "-p", "-h", peerhost, 0);
		execl ("/bin/login", "login", "-p", "-h", peerhost, 0);
		log1 ("cannot exec login");
		exit (-1);
	}
	/* Parent */
	close (slave);
	return (master);
}

int main (int argc, char **argv)
{
	struct sockaddr_in self, from;
	struct hostent *h;
	int master, n, addrlen, peeraddrlen;
	fd_set mask, m;
	struct timeval timo;

	for (;;) {
		switch (getopt (argc, argv, "vDL:")) {
		case -1:
			break;
		case 'v':
			++verbose;
			continue;
		case 'D':
			++debug;
			continue;
		case 'L':
			logfile = optarg;
			continue;
		}
		break;
	}
	time (&t0);
	signal (SIGPIPE, SIG_IGN);

	addrlen = sizeof (struct sockaddr_in);
	if (getsockname (0, (struct sockaddr*) &self, &addrlen) < 0) {
		log2 ("cannot get socket name: %s", strerror (errno));
		quit (-2);
	}
	port = ntohs (self.sin_port);
	addrlen = sizeof (struct sockaddr_in);
	if (recvfrom (0, buf, MTU, MSG_PEEK, (struct sockaddr*) &from, &addrlen) <= 0) {
		log2 ("cannot get peer address: %s", strerror (errno));
		quit (-2);
	}
	peerfrom = from;
	peeraddrlen = addrlen;

	h = gethostbyaddr ((char*) &from.sin_addr, sizeof (from.sin_addr), AF_INET);
	if (h)
		strcpy (peername, h->h_name);
	else
		log2 ("cannot get peer host name: %s", inet_ntoa (from.sin_addr));
	log3 ("connection from %s port %d", inet_ntoa (from.sin_addr),
		ntohs (from.sin_port));

	n = IPTOS_LOWDELAY;
	if (setsockopt (0, IPPROTO_IP, IP_TOS, (char*) &n, sizeof(int)) < 0)
		log1 ("setsockopt (IP_TOS) failed");

	master = forklogin (peername);
	if (master < 0)
		quit (-1);

	FD_ZERO (&mask);
	FD_SET (0, &mask);              /* socket */
	FD_SET (master, &mask);         /* pty */

	for (;;) {
		m = mask;
		timo.tv_sec = 30;
		timo.tv_usec = 0;
		switch (select (32, &m, 0, 0, &timo)) {
		case -1:
			log2 ("select failed: %s", strerror (errno));
			quit (-1);
		case 0:
			log1 ("timed out");
			quit (-1);
		default:
			break;
		}
		if (FD_ISSET (0, &m)) {
			/* new packet arrived */
			n = recvfrom (0, buf, MTU, 0, (struct sockaddr*) &from, &addrlen);
			if (n <= 0) {
				log2 ("recv failed: %s", strerror (errno));
				quit (-1);
			}
			if (addrlen != peeraddrlen ||
			    bcmp (&from, &peerfrom, sizeof (from)) != 0) {
				log3 ("strange packet from %s port %d",
					inet_ntoa (from.sin_addr),
					ntohs (from.sin_port));
				continue;
			}
			switch (*buf) {
			case DLP_DATA:
				ibytes += n-1;
				ipackets++;
				write (master, buf+1, n-1);
				break;
			case DLP_LOGOUT:
				quit (0);
			case DLP_ALIVE:
				/* log1 ("keepalive"); */
				break;
			default:
				log1 ("invalid packet");
				quit (-1);
			}
		}
		if (FD_ISSET (master, &m)) {
			/* pty input pending */
			n = read (master, buf+1, MTU-1);
			if (n < 0) {
				log2 ("pty read failed: %s", strerror (errno));
				quit (-1);
			}
			if (n == 0)
				quit (0);
			obytes += n;
			opackets++;
			*buf = DLP_DATA;
			sendto (0, buf, n+1, 0, (struct sockaddr*) &peerfrom, sizeof peerfrom);
		}
	}
}
