#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

struct termios tc, tt, ott;
char *comname = "/dev/cuaa1";
char *termname = "/dev/tty";
int speed = 9600;
int term, com;
int dtr, rts;

void quit ()
{
	if (com >= 0) {
		close (com);
		fprintf (stderr, "Disconnected.\n");
	}
	if (term >= 0) {
		tcsetattr (term, 0, &ott);
		close (term);
	}
	exit (0);
}

void run (char *av, ...)
{
	int status;
	int pid;

	pid = fork ();
	if (pid < 0) {
		perror ("fork");
		return;
	}
	if (pid == 0) {
		/* child */
		close (0);
		dup (com);
		close (1);
		dup (com);
		close (com);
		close (term);
		execvp (av, &av);
		exit (-1);
	}
	/* parent */
	signal (SIGTERM, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGINT, SIG_IGN);

	while (pid != wait (&status));

	signal (SIGTERM, quit);
	signal (SIGQUIT, quit);
	signal (SIGINT, quit);
}

int filter (int len, unsigned char *str)
{
	static int state = 0;
	static unsigned char *prefix = "\30B000000";
	int i;

	for (i=0; i<len; ++i) {
		if (str[i] == prefix[state])
			++state;
		else
			state = 0;
		if (! prefix[state]) {
			run ("rz", "-t", "10", (char*) 0);
			state = 0;
		}
	}
	return (len);
}

void showstatus ()
{
	int s;

	if (ioctl (com, TIOCMGET, &s) < 0) {
		perror ("TIOCMGET");
		return;
	}
	printf ("DTR\tRTS\tCTS\tDSR\tRING\n");
	printf ("%c\t%c\t%c\t%c\t%c\n",
		(s & TIOCM_DTR) ? '+' : '-',
		(s & TIOCM_RTS) ? '+' : '-',
		(s & TIOCM_CTS) ? '+' : '-',
		(s & TIOCM_DSR) ? '+' : '-',
		(s & TIOCM_RI) ? '+' : '-');
}

int cmd (int len, unsigned char *str)
{
	static int state = 0;
	static unsigned char *prefix = "\36c";
	int i, val;

	for (i=0; i<len; ++i) {
		if (state) {
			switch (str[i]) {
			default:
				continue;
			case 'q':
				quit ();
				break;
			case 's':
				showstatus ();
				break;
			case 'h':
				printf ("Hanging up...");
				fflush (stdout);
				ioctl (com, TIOCCDTR, 0);
				sleep (1);
				printf ("done\n");
				ioctl (com, dtr ? TIOCSDTR : TIOCCDTR, 0);
				break;
			case 'b':
				printf ("Sending break...");
				fflush (stdout);
				tcsendbreak (com, 0);
				printf ("done\n");
				break;
			case 'd':
				printf ("Set DTR = %s...",
					(dtr ^= 1) ? "ON" : "OFF");
				fflush (stdout);
				ioctl (com, dtr ? TIOCSDTR : TIOCCDTR, 0);
				printf ("done\n");
				break;
			case 'r':
				printf ("Set RTS = %s...",
					(rts ^= 1) ? "ON" : "OFF");
				fflush (stdout);
				val = (rts ? TIOCM_RTS : 0) |
					 (dtr ? TIOCM_DTR : 0);
				ioctl (com, TIOCMSET, &val);
				printf ("done\n");
				break;
			}
			state = 0;
		} else if (str[i] == prefix[state]) {
			++state;
			printf ("Command:  S-Status  B-send Break  H-Hangup  D-change DTR  R-change RTS  Q-Quit\n");
		} else
			continue;
		bcopy (str+i+1, str+i, len-i-1);
		--len;
		--i;
	}
	return (len);
}

int copy (int a, int b, char *name, int (*dofilter) ())
{
	unsigned char buf [512];
	int len;

	len = read (a, buf, sizeof (buf));
	if (len > 0) {
		if (dofilter)
		len = (*dofilter) (len, buf);
		if (len > 0)
			write (b, buf, len);
	} else if (len < 0)
		perror (name);
	else
		fprintf (stderr, "EOF on %s\n", name);
	return (len);
}

void connect (int a, int b, char *aname, char *bname)
{
	fd_set mask, m;
	int idle;

	FD_ZERO (&mask);
	FD_SET (a, &mask);
	FD_SET (b, &mask);
	dtr = rts = 1;

	for (;;) {
		m = mask;
		if (select (32, &m, 0, 0, 0) < 0) {
			perror ("select");
			return;
		}
		idle = 1;
		if (FD_ISSET (b, &m)) {
			if (copy (b, a, bname, filter) < 0)
				return;
			idle = 0;
		}
		if (FD_ISSET (a, &m)) {
			if (copy (a, b, aname, cmd) < 0)
				return;
			idle = 0;
		}
		if (idle)
			fprintf (stderr, "unexpected return from select()\n");
	}
}

void main (int argc, char **argv)
{
	if (argc > 1)
		comname = argv[1];
	if (argc > 2)
		speed = atoi (argv[2]);

	signal (SIGTERM, quit);
	signal (SIGQUIT, quit);
	signal (SIGINT, quit);

	term = open (termname, O_RDWR);
	if (term < 0) {
		perror (termname);
		exit (-1);
	}

	fprintf (stderr, "Opening `%s'...", comname);
	com = open (comname, O_RDWR);
	if (com < 0) {
		perror (comname);
		exit (-1);
	}
	fprintf (stderr, "done.\n");

	tcgetattr (com, &tc);
	cfsetspeed (&tc, speed);
	tc.c_iflag = IGNBRK | IXON | IXANY;
	tc.c_lflag = 0;
	tc.c_oflag = 0;
	tc.c_cc[VERASE] = 0;
	tc.c_cc[VKILL] = 0;
	tc.c_cc[VMIN] = 1;
	tc.c_cc[VTIME] = 0;
	tcsetattr (com, 0, &tc);

	tcgetattr (term, &tt);
	ott = tt;
	tt.c_lflag = 0;
	tt.c_iflag = IXON | IXANY;
	tc.c_oflag = 0;
	tt.c_cc[VMIN] = 1;
	tt.c_cc[VTIME] = 0;
	tcsetattr (term, 0, &tt);

	fprintf (stderr, "Connected at %d baud.  Press ^^q to quit.\n", speed);
	connect (term, com, termname, comname);

	quit ();
}
