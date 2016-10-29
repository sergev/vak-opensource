# include "param.h"
# include "svsb.h"

/*
 * Scaled down version of C Library printf.
 * Used to print diagnostic information directly on console tty.
 * Since it is not interrupt driven, all system activities are
 * suspended.  Printf should not be used for chit-chat.
 *
 * One additional format: %b is supported to decode error registers.
 * Usage is:
 *	printf("reg=%b\n", regval, "<base><arg>*");
 * Where <base> is the output base expressed as a control character,
 * e.g. \10 gives octal; \20 gives hex.  Each arg is a sequence of
 * characters, the first of which gives the bit number to be inspected
 * (origin 1), and the next characters (up to a control character, i.e.
 * a character <= 32), give the name of the register.  Thus
 *	printf("reg=%b\n", 3, "\10\2BITTWO\1BITONE\n");
 * would produce output:
 *	reg=2<BITTWO,BITONE>
 */

/*VARARGS1*/
printf(fmt, x1)
char *fmt;
unsigned x1;
{
	prf(fmt, &x1);
}

prf(fmt, adx)
register char *fmt;
register u_int *adx;
{
	register int b, c, i;
	char *s;
	int any;

loop:
	while ((c = *fmt++) != '%') {
		if(c == '\0')
			return;
		putchar(c);
	}
again:
	c = *fmt++;
	/* THIS CODE IS ELBRUS-B DEPENDENT IN HANDLING %l? AND %c */
	switch (c) {

	case 'l':
		goto again;
	case 'x': case 'X':
		b = 16;
		goto number;
	case 'd': case 'D':
	case 'u':		/* what a joke */
		b = 10;
		goto number;
	case 'o': case 'O':
		b = 8;
number:
		printn((u_long)*adx, b);
		break;
	case 'c':
		b = *adx;
		for (i=0; i<64; i+=8)
			if (c = (b >> i) & 0xff)
				putchar(c);
		break;
	case 'w':
		printw (*adx);
		break;
	case 'b':
		b = *adx++;
		s = (char *)*adx;
		printn((u_long)b, *s++);
		any = 0;
		if (b) {
			while (i = *s++) {
				if (b & (1 << (i-1))) {
					putchar(any? ',' : '<');
					any = 1;
					for (; (c = *s) > ' '; s++)
						putchar(c);
				} else
					for (; *s > ' '; s++)
						;
			}
			if (any)
				putchar('>');
		}
		break;

	case 's':
		s = (char *)*adx;
		while (c = *s++)
			putchar(c);
		break;
	}
	adx++;
	goto loop;
}

/*
 * Printn prints a number n in base b.
 * We don't use recursion to avoid deep kernel stacks.
 */
printn(n, b)
u_long n;
{
	char prbuf[11];
	register char *cp;

	if (b == 10 && (int)n < 0) {
		putchar('-');
		n = (unsigned)(-(int)n);
	}
	cp = prbuf;
	do {
		*cp++ = "0123456789abcdef"[n%b];
		n /= b;
	} while (n);
	do
		putchar(*--cp);
	while (cp > prbuf);
}

print4x (w)
{
	register n;

	for (n=12; n>=0; n-=4)
		putchar ("0123456789abcdef" [w>>n & 0xf]);
}

printw (w)
{
	print4x (w >> 48);
	putchar (' ');
	print4x (w >> 32);
	putchar (' ');
	print4x (w >> 16);
	putchar (' ');
	print4x (w);
}

/*
 * Print a character on console.
 */
putchar (c)
{
	switch (c) {
	case '\b':
	case '\r':
	case '\n':
	case '\t':
		break;
	default:
		if (c == 0177)
			putchar ('^');
		else if (c == 0377)
			putchar ('~');
		else if (c >= ' ')
			break;
		else
			putchar ('^');
		c = c & 037 | 0100;
	}
	while (_ttyout)
		idle (1);
	_ttyout = c;
	_flush_ ();                     /* выталкивание БРЗ */
	if (c == '\n')
		putchar ('\r');
}

getchar ()
{
	register c;

	while (! _ttyin)
		idle (0);
	c = _ttyin;
	if (c == ctrl('^')) {           /* restart boot */
		_ttyin = 0;
		_rtt ();
	}
	_ttyin = 0;
	_flush_ ();                     /* выталкивание БРЗ */
	if (c == '\r')
		c = '\n';
	putchar (c);
	return (c);
}

gets (buf)
char *buf;
{
	register char *lp;
	register c;

	lp = buf;
	for (;;) {
		c = getchar () & 0377;
		switch (c) {
		case ctrl ('r'):        /* reprint line */
			*lp = '\0';
			putchar ('\n');
			for (lp=buf; *lp; ++lp)
				putchar (*lp);
			continue;
		case '\n':
		case '\r':              /* end of line */
			*lp++ = '\0';
			return;
		case '\b':              /* backspace */
			if (lp > buf) {
				lp--;
				putchar (' ');
				putchar ('\b');
				if (*lp==0377 || *lp==0177 || *lp<' ') {
					putchar ('\b');
					putchar (' ');
					putchar ('\b');
				}
			} else
				putchar (' ');
			continue;
		case '#':               /* delete prev char */
		case '\177':
		case '\377':
			if (lp > buf) {
				lp--;
				putchar (*lp);
			}
			continue;
		case '@':               /* kill string */
		case ctrl ('u'):
			lp = buf;
			putchar ('\n');
			continue;
		default:
			*lp++ = c;
		}
	}
}
