# include "deftns.h"

# include <stdio.h>
# include <conio.h>
# include <malloc.h>

# define ctrl_on(p)             out (ic_ctrl_port, ic_ctrl |= (p))
# define ctrl_off(p)            out (ic_ctrl_port, ic_ctrl &= ~(p))
# define strobe()               { ctrl_on (STROBE); ctrl_off (STROBE); }
# define put_data(p)            { out (ic_data_port, ~(p)); strobe (); }

# define gch()                  (bytes ? bytes--, *cp++ & 0377 : _gch ())

typedef union {
	unsigned long l;
	unsigned i [2];
	unsigned char c [4];
} hword;

unsigned ic_data_port, ic_ctrl_port, ic_status_port, ic_ctrl;
unsigned long start_addr, size, cur_addr;
hword w1, w2;
int bytes = 0, c;

FILE *fp;
char *cp, *buf;
int eof = 0;
size_t bufsize;

_gch ()
{
	if (eof)
		return (0);
	bytes = fread (buf, 1, bufsize, fp);
	if (bytes == -1) {
		fputs ("\nCannot read.", stderr);
		exit (2);
	}
	if (bytes)
		cp = buf;
	else {
		eof++;
		return 0;
	}
	bytes--;
	return (*cp++ & 0xFF);
}

msg (s)
long s;
{
	printf ("%06lx words OK.\r", s);
}

main (argc, argv)
char **argv;
{
	long stored = 0, disk_addr;

	switch (argc) {
	case 1:
		fp = stdin;
		break;
	case 2:
		if (( fp = fopen (argv[1], "rb")) == (FILE *) NULL) {
			perror (argv[1]);
			exit (3);
		}
		break;
	default :
		fputs ("Invalid number of arguments", stderr);
		exit (4);
	}
	init_ic ();
	get_buf ();
	start_addr = DP_MEM;
	printf ("start address = %07lx\n", start_addr);
	printf ("Storing...\n");
	msg (stored);
	for (cur_addr=start_addr; !eof; cur_addr++) {
		get_word ();
		store_word (w1.l, w2.l, cur_addr);
		++stored;
		if ((stored & 0xF) == 0)
			msg (stored);
	}
	msg (stored);
	printf ("\nDone.\n");
	store_word (0L, (long) SA_MODE, (long) DP_DISK);
	store_word (0L, (long) SA_DIR, (long) DP_WRITE);
	store_word (0L, (long) SA_CHAN, (long) DP_CHAN);
	store_word (0L, (long) SA_UNIT, (long) DP_UNIT);
	store_word (0L, (long) SA_MEM, (long) DP_MEM);
	store_word (0L, (long) SA_LENGTH, (long) stored);
	fputs ("Disk address - ", stdout);
	scanf ("%lx", &disk_addr);
	store_word (0L, (long) SA_BLOCK, (long) disk_addr);
}

get_buf ()
{
	for (bufsize=0xFFF0; ; bufsize-=16)
		if ((buf = malloc (bufsize)) != NULL) {
			bufsize -= 1000;
			return;
		}
}

init_ic ()
{
	int far *fip;
	int *ip, base;

	ip = (int *) &fip;
	ip [0] = 0x408;
	ip [1] = 0;
	base = *fip;
	ic_data_port = base + IC_DATA;
	ic_ctrl_port = base + IC_CTRL;
	ic_status_port = base + IC_STATUS;
	ctrl_on (INIT);
}

get_word ()
{
	register char *wcp;

	wcp = & w1.c [3];
	*wcp-- = gch ();
	*wcp-- = gch ();
	*wcp-- = gch ();
	*wcp-- = gch ();
	wcp = & w2.c [3];
	*wcp-- = gch ();
	*wcp-- = gch ();
	*wcp-- = gch ();
	*wcp-- = gch ();
}

do_parity (w)
register long w;
{
	w ^= w >> 16;
	w ^= w >> 8;
	w ^= w >> 4;
	w ^= w >> 2;
	w ^= w >> 1;
	return (w & 1);
}

store_word (l, r, a)
long l, r, a;
{
	char tag_byte;
	register i;
	register char *wcp;
	hword waddr;

	waddr.l = a | 0x4000000l;
# ifdef DEBUG
	printf ("\333\333\333  <%07lx> := %08lx%08lx;\n",
		a, l, r);
# endif
	waddr.c [3] |= do_parity (waddr.l & 0x40003FFl) << 3 |
		   do_parity (waddr.l & 0x07FFC00l) << 4;
	for (i=3, wcp= &waddr.c[3]; i>=0; --i) {
		ctrl_off (INIT);
		ctrl_on (INIT);
		put_data (*wcp--);
		put_data (0357);
		put_data( 0374);
	}
	tag_byte = do_parity (l) << 6 | do_parity (r) << 5;
		ctrl_off (INIT);
		ctrl_on (INIT);
		put_data (tag_byte);
		put_data (0357);
		put_data (0376);
	w1.l = l;
	for (i=3, wcp= &w1.c[3]; i>=0; --i) {
		ctrl_off (INIT);
		ctrl_on (INIT);
		put_data (*wcp--);
		put_data (0357);
		put_data (0376);
	}
	w2.l = r;
	for (i=3, wcp= &w2.c[3]; i>=0; --i) {
		ctrl_off (INIT);
		ctrl_on (INIT);
		put_data (*wcp--);
		put_data (0357);
		put_data (0376);
	}
	ctrl_off (INIT);
	ctrl_on( INIT);
	put_data (0001);
	put_data (0353);
	/* for (i=1000; i; --i); */
}
