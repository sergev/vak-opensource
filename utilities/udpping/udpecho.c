/*
 * UDP echo server.
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
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define VERSION		"1.0"

/* Use the echo-port as default */
#define DEFAULT_PORT	7777

unsigned long total_packets [8], last_packets [8], last_bytes [8];
unsigned long long total_bytes [8];

struct timeval t0;

void sig_handler (int signum)
{
	printf ("\nInterrupted.\n");
	exit (0);
}

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

int send_packet (int sck, void *buf, int bufsz, struct sockaddr_in addr,
	int tos)
{
	int cc;
	static int prev_tos = -1;

	if (prev_tos != tos) {
		if (setsockopt (sck, SOL_IP, IP_TOS, &tos, sizeof(tos)) < 0) {
			perror ("setsockopt (IP_TOS)");
			exit (1);
		}
		prev_tos = tos;
	}
	cc = sendto (sck, buf, bufsz, 0,
		(struct sockaddr*) &addr, sizeof (addr));
	return cc;
}

void display (int signum)
{
	struct timeval now;
	unsigned long sec, bps;
	int k;

	gettimeofday (&now, 0);
	sec = now.tv_sec - t0.tv_sec;

	printf ("\33[H\33[J");
	printf ("UDP Echo Server, Version " VERSION "\n\n");
	printf ("Time passed: %02ld:%02ld:%02ld\n\n",
		sec / 60 / 60, sec / 60 % 60, sec % 60);

	printf ("Priority   Packets          Bytes  Packets/sec   Bits/sec\n");
	printf ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	for (k=0; k<8; ++k) {
		printf ("%5d %10ld %15lld %10ld ",
			k, total_packets[k], total_bytes[k], last_packets[k]);

		bps = last_bytes[k] * 8;
		if (bps >= 10000000)
			printf ("%10ld M\n", bps / 1000000);
		else if (bps >= 10000)
			printf ("%10ld k\n", bps / 1000);
		else
			printf ("%10ld\n", bps);

		last_packets[k] = 0;
		last_bytes[k] = 0;
	}
}

int main ()
{
	struct sockaddr_in addr;
	struct itimerval itv;
	int sck, cc, tos, prio;
	char buf [4096];

	sck = socket (PF_INET, SOCK_DGRAM, 0);
	if (sck < 0) {
		perror ("Cannot create socket");
		exit (1);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl (INADDR_ANY);
	addr.sin_port = htons (DEFAULT_PORT);
	if (bind (sck, (struct sockaddr*) &addr, sizeof (addr)) < 0) {
		perror ("Cannot assign address");
		exit (1);
	}

	/* Check our privileges. */
	tos = 7 << 5;
	if (setsockopt (sck, SOL_IP, IP_TOS, &tos, sizeof(tos)) < 0) {
		perror ("setsockopt (IP_TOS)");
		exit (1);
	}

	/* Enable receiving TOS. */
	tos = 1;
	if (setsockopt (sck, SOL_IP, IP_RECVTOS, &tos, sizeof(tos)) < 0) {
		perror ("setsockopt (IP_RECVTOS)");
		exit (1);
	}

	signal (SIGINT, sig_handler);
	signal (SIGALRM, display);

	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 10000;
	if (setitimer (ITIMER_REAL, &itv, 0) < 0) {
		perror ("setitimer");
		exit (1);
	}
	gettimeofday (&t0, 0);
	for (;;) {
		cc = receive_packet (sck, buf, sizeof(buf), &addr, &tos);
		/*printf ("%d bytes from %s, TOS=0x%02x\n",
			cc, inet_ntoa (addr.sin_addr), tos);*/

		prio = tos >> 5;
		++total_packets[prio];
		++last_packets[prio];
		total_bytes[prio] += 14+4 + 20 + 8 + cc; /* add headers */
		last_bytes[prio] += 14+4 + 20 + 8 + cc;
#if 0
		if (cc > 0) {
			cc = send_packet (sck, buf, cc, addr, tos);
			if (cc < 0)
				perror ("sendto");
		}
#endif
	}
}
