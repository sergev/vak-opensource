#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include "config.h"
#include "ut.h"

char home[MAXPATHLEN];
int homelen;

char buf[1024];

char *filter (char *str)
{
	char *p = str, *q;

	if (strncmp (str, home, homelen) == 0)
		p += homelen;
	if (p[0]=='/' && p[1]=='.' && p[2]>='0' && p[2]<='9' && p[3]=='/')
		p += 3;
	q = strrchr (p, '/');
	if (q && q[1]=='.' && q[2]==0) {
		q[1] = 0;
		if (q > p)
			q[0] = 0;
	}
	return (p);
}

int main (int argc, char **argv)
{
	int c;
	char *p;

	if (argc < 3) {
		fprintf (stderr, "Usage: logmessage progname message...\n");
		exit (1);
	}
	openlog ("mlog", LOG_PERROR, LOG_MAIL);

	/* catch signals */
	sigcatch ();

	/* enter public directory */
	if (chdir (PUBLICDIR) < 0) {
		syslog (LOG_ERR, "cannot chdir to %s", PUBLICDIR);
		exit (-1);
	}
	if (! getwd (home)) {
		syslog (LOG_ERR, "cannot get %s directory name", PUBLICDIR);
		exit (-1);
	}
	homelen = strlen (home);

	strcpy (buf, argv[1]);
	p = buf;
	for (c=2; c<argc; ++c) {
		p += strlen(p);
		if (p-buf > sizeof(buf) - 20)
			break;
		sprintf (p, " %s", filter (argv[c]));
	}
	syslog (LOG_INFO, "%s", buf);
	return (0);
}
