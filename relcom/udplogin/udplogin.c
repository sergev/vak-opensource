/*
 * Remote login utility, based on UDP protocol.
 *
 * Copyright (C) 1994 Cronyx Ltd.
 * Authors: Serge Vakulenko, <vak@cronyx.msk.su>
 *          Sergey Ryzhkov, <sir@cronyx.msk.su>
 * Date: Wed Jul 27 21:27:58 MSD 1994
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include "dlp.h"

#define MTU             (8*1024)        /* maximum packet size */

struct termios tc, tt, ott;
char buf[MTU];
int sock;
int qchar = ('^' & 037);
int verbose;
int debug;

extern char *optarg;
extern int optind;

void quit (int err)
{
	char packet[1];

	*packet = DLP_LOGOUT;
	send (sock, packet, 1, 0);
	shutdown (sock, 2);
	tcsetattr (0, 0, &ott);
	printf ("Disconnected.\n");
	exit (err);
}

char *symbol (int c)
{
	static char str[3];

	if (c<' ' || c==127) {
		str[0] = '^';
		str[1] = (c & 037) + '@';
		str[2] = 0;
	} else {
		str[0] = c;
		str[1] = 0;
	}
	return (str);
}

struct in_addr getaddr (char *addr)
{
	struct in_addr ipaddr;

	if (*addr>='0' && *addr<='9') {
		ipaddr.s_addr = inet_addr (addr);
	} else {
		struct hostent *hst;

		hst = gethostbyname (addr);
		if (hst == 0) {
			fprintf (stderr, "Unknown host: %s\n", addr);
			exit (1);
		}
		ipaddr.s_addr = *(long*) hst->h_addr;
	}
	return (ipaddr);
}

void keepalive ()
{
	char packet[1];

	*packet = DLP_ALIVE;
	send (sock, packet, 1, 0);
}

void main (int argc, char **argv)
{
	struct sockaddr_in local, remote;
	fd_set mask, m;
	char *p, *host;
	int n, port = 8000;
	struct itimerval itv;

	for (;;) {
		switch (getopt (argc, argv, "vDe:")) {
		case -1:
			break;
		case 'v':
			++verbose;
			continue;
		case 'D':
			++debug;
			continue;
		case 'e':
			qchar = *optarg=='-' ? 0 :
				*optarg=='^' ? (optarg[1]&037) : *optarg;
			continue;
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (argc < 1) {
		fprintf (stderr, "Usage: udplogin host [port]\n");
		exit (-1);
	}
	host = *argv++;
	if (argc > 1)
		port = atoi (*argv);

	sock = socket (AF_INET, SOCK_DGRAM, 0);

	local.sin_family = remote.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = 0;
	remote.sin_addr = getaddr (host);
	remote.sin_port = htons (port);
	if (bind (sock, (struct sockaddr*) &local, sizeof local) < 0) {
		perror ("bind");
		exit (-1);
	}
	n = IPTOS_LOWDELAY;
	if (setsockopt (sock, IPPROTO_IP, IP_TOS, (char*) &n, sizeof(int)) < 0)
		perror ("setsockopt(IP_TOS)");

	signal (SIGTERM, quit);
	signal (SIGQUIT, quit);
	signal (SIGINT, quit);
	signal (SIGALRM, keepalive);
	signal (SIGPIPE, SIG_IGN);

	tcgetattr (0, &tt);
	ott = tt;
	tt.c_lflag = 0;
	tt.c_iflag = IXON | IXANY;
	tc.c_oflag = 0;
	tt.c_cc[VMIN] = 1;
	tt.c_cc[VTIME] = 0;
	tcsetattr (0, 0, &tt);

	fprintf (stderr, "Connected to %s.\n", host);
	if (qchar)
		fprintf (stderr, "Press %s to disconnect.\n", symbol (qchar));

	*buf = DLP_ALIVE;
	if (sendto (sock, buf, 1, 0, (struct sockaddr*) &remote, sizeof remote) < 0) {
		perror ("sendto (alive)");
		quit (-1);
	}

	n = sizeof (remote);
	if (recvfrom (sock, buf, MTU, MSG_PEEK, (struct sockaddr*) &remote, &n) < 0) {
		perror ("recvfrom");
		quit (-1);
	}
	if (connect (sock, (struct sockaddr*) &remote, sizeof remote) < 0) {
		perror ("connect");
		quit (-1);
	}

	itv.it_value.tv_sec = 10;
	itv.it_value.tv_usec = 0;
	itv.it_interval = itv.it_value;
	setitimer (ITIMER_REAL, &itv, 0);

	FD_ZERO (&mask);
	FD_SET (0, &mask);
	FD_SET (sock, &mask);

	for (;;) {
		m = mask;
		if (select (32, &m, 0, 0, 0) < 0) {
			if (errno == EINTR)
				continue;
			perror ("select");
			quit (-1);
		}
		if (FD_ISSET (0, &m)) {
			/* tty input pending */
			n = read (0, buf+1, MTU-1);
			if (qchar)
				for (p=buf+1; p<=buf+n; ++p)
					if (*p == qchar)
						quit (0);
			*buf = DLP_DATA;
			if (n>0 && send (sock, buf, n+1, 0) < 0)
				perror ("send");
		}
		if (FD_ISSET (sock, &m)) {
			/* new packet arrived */
			n = recv (sock, buf, MTU, 0);
			if (n <= 0) {
				perror ("recv failed");
				continue;
			}
			switch (*buf) {
			case DLP_DATA:
				write (0, buf+1, n-1);
				break;
			case DLP_LOGOUT:
				shutdown (sock, 2);
				quit (0);
			case DLP_ALIVE:
				/* should not happen */
				/* fprintf (stderr, "keepalive\n"); */
				break;
			default:
				fprintf (stderr, "invalid packet: %c\n", *buf);
				break;
			}
		}
	}
}
