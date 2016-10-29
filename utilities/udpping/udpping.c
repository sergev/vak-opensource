/*
 * UDP ping utility.
 *
 * Copyright (C) 2004 Serge Vakulenko <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <strings.h>
#include <time.h>

/* Name of package */
#define PACKAGE "udpping"

/* Version number of package */
#define VERSION "0.3"

/* Use the echo-port as default */
#define DEFAULT_PORT	7777

/* Use this priority as default */
#define DEFAULT_PRIORITY 0

/* Use this packet size as default */
#define DEFAULT_PACKETSZ 1024

/* Use this rate as default */
#define DEFAULT_BPS	64000

pthread_t tid;
int sck;		/* The connected socket */
char outpack [1500];	/* The data to send() */

int datalen;
long bps;
long nreceived, ntransmitted;
unsigned long long tx_bytes, rx_bytes;
long tx_msec, tx_t0;

int receive_packet (int sck, void *buf, int bufsz, struct sockaddr_in *addr,
	int *tos)
{
	struct msghdr msg;
	struct iovec iov;
	char cmsgbuf [CMSG_SPACE(100)];
	struct cmsghdr *m;
	int cc;

	iov.iov_base = buf;
	iov.iov_len = bufsz;

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;
	msg.msg_name = (struct sockaddr*) addr;
	msg.msg_namelen = sizeof (*addr);
	msg.msg_control = cmsgbuf;
	msg.msg_controllen = sizeof (cmsgbuf);

	cc = recvmsg (sck, &msg, 0);
	if (cc < 0) {
		perror ("Error receiving packet");
		exit (1);
	}
	if (tos)
		*tos = 0;
	m = (struct cmsghdr*) &cmsgbuf;
	for (m=CMSG_FIRSTHDR(&msg); m; m=CMSG_NXTHDR(&msg,m)) {
		if (m->cmsg_len == 0) {
			perror ("Null control message");
			exit (1);
		}
		if (m->cmsg_level == IPPROTO_IP &&
		    m->cmsg_type == IP_TOS) {
			if (tos)
				*tos = CMSG_DATA(m)[0];
		}
	}
	return cc;
}

void *receiver (void *arg)
{
	int cc, tos;
	unsigned char reply [1500];

	for (;;) {
		cc = receive_packet (sck, &reply, sizeof (reply), 0, &tos);
		if (cc <= 0) {
			if (errno != EINTR && errno != 2) {
				perror ("recv");
				fprintf (stderr, "errno=%d\n", errno);
			}
			continue;
		}
		++nreceived;
		rx_bytes += 14 + 4 + 20 + 8 + cc;
		/*printf ("%d bytes to %x-%x-%x-%x-%x-%x from %x-%x-%x-%x-%x-%x type %x-%x data %x-%x...\n", cc,
			reply[0], reply[1], reply[2], reply[3],
			reply[4], reply[5], reply[6], reply[7],
			reply[8], reply[9], reply[10], reply[11],
			reply[12], reply[13], reply[14], reply[15]);*/
	}
}

void *transmitter (void *arg)
{
	struct timeval tv;
	int cc;

	gettimeofday (&tv, 0);
	tx_t0 = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	for (;;) {
		cc = send (sck, outpack, datalen, 0);
		++ntransmitted;
		tx_bytes += 14 + 4 + 20 + 8 + datalen;
		for (;;) {
			gettimeofday (&tv, 0);
			tx_msec = tv.tv_sec * 1000 + tv.tv_usec / 1000;
			tx_msec -= tx_t0;
			if (tx_msec <= 0)
				tx_msec = 1;

			/* Sleep until rate becomes small enough. */
			if (tx_bytes * 8000 / tx_msec <= bps)
				break;

			usleep (10000);
		}
		if (cc < 0)  {
			perror ("send");
			usleep (10000);
			continue;
		}
		if (cc != datalen)  {
			printf ("udpping: send %d chars, ret=%d\n",
				datalen, cc);
		}
	}
}

/*
 *  Set the abort-flag on receiveing a SIGHUP or SIGINT
 */
void sig_handler (int sig)
{
	printf ("\nStopped.\n");
	exit (0);
}

/*
 * Show the usage-text to the user
 */
void usage (void)
{
	printf ("%s: An UDP based logging uptime checker\n\n", PACKAGE);
	printf ("Usage: %s [options] host\n", PACKAGE);
	printf ("  -h          Show this help\n");
	printf ("  -V          Show program version\n");
	printf ("  -p PORT     Send packets to PORT, default is 7\n");
	printf ("  -t PRIO     Set precedence, 0..7, default is 0\n");
	printf ("  -s bytes    Set packet size, default is 1024\n");
	printf ("  -r bps      Set bit rate, default is %d\n", DEFAULT_BPS);
	printf ("\n Please report bugs to mats@nerdlabs.org\n");
	exit (1);
}

int main (int argc, char *argv[])
{
	struct sockaddr_in addr;
	struct sched_param sched;
	struct hostent *he;
	int port, c, priority, tos;
	long nreceived_last, ntransmitted_last;
	unsigned long long tx_bytes_last, rx_bytes_last;
	long tx_msec_last;

	/*
	 * Initialize variables that can be modified by the command line
	 * options
	 */
	port = DEFAULT_PORT;
	priority = DEFAULT_PRIORITY;
	datalen = DEFAULT_PACKETSZ;
	bps = DEFAULT_BPS;

	/* Parse the command line options */
	while ((c = getopt (argc, argv, "hVp:t:s:r:")) != -1) {
		switch (c) {
		case 'p':
			port = strtol (optarg, 0, 0);
			break;

		case 't':
			priority = strtol (optarg, 0, 0);
			break;

		case 's':
			datalen = strtol (optarg, 0, 0);
			if (datalen > sizeof (outpack)) {
				printf ("Too big packet size, max is %d\n",
					sizeof (outpack));
				exit (1);
			}
			break;

		case 'r':
			bps = strtol (optarg, 0, 0);
			break;

		case 'V':
			printf ("%s version %s\n", PACKAGE, VERSION);
			exit (1);
			break;

		case 'h':
		default:
			usage ();
			break;
		}
	}

	/*
	 * Now, take care of the host argument. This version only handles one
	 * host
	 */
	if (argc - optind < 1) {
		printf ("Host to check is missing.\nType %s -h for help\n", PACKAGE);
		exit (1);
	}
	if (argc - optind > 1) {
		printf ("Multiple hosts not supported.\nType %s -h for help\n", PACKAGE);
		exit (1);
	}

	he = gethostbyname (argv[optind]);
	if (! he) {
		printf ("Can't resolve host %s\n", argv[optind]);
		exit (1);
	}

	/* Create and (pseudo-) connect a UDP-socket */
	bzero (&addr, sizeof (addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons (port);
	addr.sin_addr.s_addr = *(in_addr_t*) he->h_addr;

	sck = socket (AF_INET, SOCK_DGRAM, 0);
	if (sck < 0) {
		perror ("Error creating socket");
		exit (1);
	}
	if (connect (sck, (struct sockaddr*) &addr, sizeof (addr)) < 0) {
		perror ("Error in connect()");
		exit (1);
	}

	tos = 1;
	if (setsockopt (sck, SOL_IP, IP_RECVTOS, &tos, sizeof(tos)) < 0) {
		perror ("setsockopt (IP_RECVTOS)");
		exit (1);
	}
	tos = priority << 5;
	if (setsockopt (sck, SOL_IP, IP_TOS, &tos, sizeof(tos)) < 0) {
		perror ("setsockopt (IP_TOS)");
		exit (1);
	}
#if 0
	if (setsockopt (sck, SOL_SOCKET, SO_PRIORITY, &priority,
	    sizeof(priority)) < 0) {
		perror ("setsockopt (SO_PRIORITY)");
		exit (1);
	}
#endif
	/*
	 * Copy the date and time to the data collection string and
	 * initialize the request value to be sent
	 */
	memset (outpack, datalen, 'Z');

	if (sched_getparam (0, &sched) != 0) {
		perror ("sched_getparam");
		exit (1);
	}

	/* Start receiver. */
	if (0 != pthread_create (&tid, 0, receiver, 0)) {
		perror ("pthread_start");
		exit (1);
	}

	if (geteuid() == 0) {
		/* Increase receiver priority. */
		sched.sched_priority += 10;
		if (pthread_setschedparam (tid, SCHED_FIFO, &sched) != 0) {
			perror ("pthread_setschedparam");
			exit (1);
		}
		sched.sched_priority -= 2;
		if (pthread_setschedparam (pthread_self(), SCHED_FIFO, &sched) != 0) {
			perror ("pthread_setschedparam");
			exit (1);
		}
	}

	/* Start transmitter. */
	if (0 != pthread_create (&tid, 0, transmitter, 0)) {
		perror ("pthread_start");
		exit (1);
	}

	/* Set up a signal handler for SIGHUP's or SIGINT's */
	signal (SIGINT, sig_handler);
	setlinebuf (stdout);

	printf ("%-32s%s\n", "--------- Packets ---------", "------- Bits/sec -------");
	printf ("%-16s%-16s%-16s%s\n", "Transmitted", "Lost", "Transmit", "Receive");
	for (;;) {
		ntransmitted_last = ntransmitted;
		nreceived_last    = nreceived;
		tx_bytes_last     = tx_bytes;
		rx_bytes_last     = rx_bytes;
		tx_msec_last      = tx_msec;

		sleep (1);
		if (tx_msec == tx_msec_last)
			continue;
		printf ("%-16ld%-16ld%-16ld%ld\n",
			ntransmitted - ntransmitted_last,
			ntransmitted - ntransmitted_last -
				(nreceived - nreceived_last),
			(long) ((tx_bytes - tx_bytes_last) * 8000 /
				(tx_msec - tx_msec_last)),
			(long) ((rx_bytes - rx_bytes_last) * 8000 /
				(tx_msec - tx_msec_last)));
	}
}
