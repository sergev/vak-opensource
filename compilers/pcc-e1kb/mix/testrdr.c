
# include <stdio.h>
# include <ctype.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

# define W 8                    /* длина слова в байтах */

struct tsthdr {                 /* заголовок */
	int magic;              /* 1 byte */
	long addr;              /* 4 bytes */
	long len;               /* 4 bytes */
} hdr;

FILE *text;

# define MSG(l,r) (msg ? (r) : (l))

char msg;
int rflag;

initmsg ()
{
	register char *p;
	extern char *getenv ();

	msg = (p = getenv ("MSG")) && *p == 'r';
}

main (argc, argv)
register char **argv;
{
	int yesarg;     /* были ли параметры - имена файлов */

	initmsg ();
	while(--argc) {
		++argv;
		if (**argv == '-') {
			register char *cp;

			for (cp = *argv+1; *cp; cp++) switch (*cp) {
			case 'r':
				++rflag;
				break;
			}
		} else {
			show (*argv);
			yesarg = 1;
		}
	}
	if (! yesarg) show ((char *) 0);
	return (0);
}

show (fname)
register char *fname;
{
	if (! fname)
		text = stdin;
	else if (! (text = fopen (fname, "r"))) {
		printf (MSG ("show: %s not found\n",
				"show: %s не найден\n"),
			fname);
		return;
	}
	fgettsthdr (text, &hdr);
	if (feof (text)) {
		printf (MSG ("show: %s not a test file\n",
				"show: %s не тестовый файл\n"),
			fname);
		return;
	}
# ifdef notdef
	if (! fname)
		printf ("; File \"%s\"\n", fname);
	printf (";\n");
	printf ("; Magic %xh Address %lxh Length %lxh End %lxh\n",
		hdr.magic, hdr.addr, hdr.len,  hdr.addr+hdr.len);
	printf (";\n");
# endif
	showfile (hdr.addr, hdr.len);
}

long frevgeth (f)
register FILE *f;
{
	register long h;

	h = (long) getc (f) << 24;
	h |= (long) getc (f) << 16;
	h |= getc (f) << 8;
	h |= getc (f);
	return (h);
}

frevputh (h, f)
register long h;
register FILE *f;
{
	putc ((int) (h >> 24), f);
	putc ((int) (h >> 16), f);
	putc ((int) (h >> 8), f);
	putc ((int) h, f);
}

fgettsthdr (text, h)
register FILE *text;
register struct tsthdr *h;
{
	h->magic = getc (text);
	h->addr = frevgeth (text);
	h->len = frevgeth (text);
}

showfile (addr, len)
long addr, len;
{
	register t;
	register long l, r;

	while (len--) {
		t = getc (text);
		l = frevgeth (text);
		r = frevgeth (text);
		if (rflag) {
			frevputh (l, stdout);
			frevputh (r, stdout);
		} else {
			fputh (r, stdout);
			fputh (l, stdout);
		}
	}
}
