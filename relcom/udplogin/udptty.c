#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LEN 4096

extern int errno;

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
		ipaddr.s_addr = *(u_long *) hst->h_addr;
	}
	return ipaddr;
}

int main (int argc, char *argv[])
{
	int fd, nbyte, rc, sock, lenout, lenout1, i;
	struct sockaddr_in local, remote;
	char buf[LEN];

	if (argc != 8) {
		fprintf (stderr, "Usage:\n");
		fprintf (stderr, "\tudptty ttyname myaddr myport peeraddr peerport maxsend maxrecv\n");
		return 1;
	}
	local.sin_family = remote.sin_family = AF_INET;
	local.sin_addr = getaddr (argv[2]);
	local.sin_port = htons ((short) atoi (argv[3]));
	remote.sin_addr = getaddr (argv[4]);
	remote.sin_port = htons ((short) atoi (argv[5]));
	lenout = atoi (argv[6]);
	lenout1 = atoi (argv[7]);
	sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (bind (sock, (struct sockaddr*) &local, sizeof local) < 0) {
		perror ("bind");
		return 1;
	}
	if (connect (sock, (struct sockaddr*) &remote, sizeof remote) < 0) {
		perror ("connect");
		return 1;
	}
	fd = open (argv[1], O_RDWR);
	if (fd < 0) {
		perror (argv[1]);
		return 1;
	}
	rc = fork ();
	if (rc < 0) {
		perror ("fork");
		return 1;
	}
	if (rc) {
		while (1) {
			nbyte = read (fd, buf, LEN);
			if (nbyte < 0) {
				perror ("read");
				continue;
			}
			if (! nbyte)
				continue;
			for (i=0; i<nbyte; i+=lenout) {
				int j = nbyte - i;

				if (j > lenout)
					j = lenout;
				if (write (sock, buf + i, j) < 0)
					perror ("write");
			}
		}
	} else {
		while (1) {
			struct sockaddr_in from;
			int addrlen;

			addrlen = sizeof from;
			nbyte = recvfrom (sock, buf, LEN, 0,
				(struct sockaddr*) &from, &addrlen);
			if (nbyte < 0)
				perror ("recvfrom");
			if (from.sin_port != remote.sin_port ||
			    from.sin_addr.s_addr != remote.sin_addr.s_addr)
				continue;
			if (! nbyte)
				continue;
			for (i=0; i<nbyte; i+=lenout1) {
				int j = nbyte - i;

				if (j > lenout1)
					j = lenout1;
				if (write (fd, buf + i, j) < 0)
					perror ("write");
			}
		}
	}
}
