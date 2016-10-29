# include <stdio.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

# define MSG(l,r) (msg ? (r) : (l))

struct word {
	int sum;                /* 1 byte */
	long left;              /* 4 bytes */
	long right;             /* 4 bytes */
} word;

char msg;
long address = 1;
long size;
struct exec hdr;        /* заголовок */

long fgeth (f)
register FILE *f;
{
	register long h;

	h = getc (f);
	h |= getc (f) << 8;
	h |= (long) getc (f) << 16;
	return (h | (long) getc (f) << 24);
}

initmsg ()
{
	register char *p;
	extern char *getenv ();

	msg = (p = getenv ("MSG")) && *p == 'r';
}

error (s, a, b, c)
long a, b, c;
{
	fprintf (stderr, "tld: ");
	fprintf (stderr, s, a, b, c);
	fprintf (stderr, "\n");
	exit (1);
}

main (argc, argv)
register char *argv[];
{
	register short i;
	register char *cp;
	char *infile = 0, *outfile = "tst.out";
	int ofile = 0;

	initmsg ();
	if (argc == 1)
		error (MSG ("Usage: %s [-aaddress] [-o file] file\n",
			"Вызов: %s [-aадрес] [-o файл] файл...\n"), argv [0]);

	/* разбор флагов */

	for (i=1; i<argc; i++) switch (argv[i][0]) {
	case '-':
		for (cp=argv[i]; *cp; cp++) switch (*cp) {
		case 'a':
			if (cp [1]) {
				sscanf (cp+1, "%ld", &address);
				while (*++cp);
				--cp;
			} else if (i+1 < argc)
				sscanf (argv [++i], "%ld", &address);
			address &= 0x7fffff;
			break;
		case 'o':       /* выходной файл */
			if (ofile)
				error (MSG ("too many -o flags",
					"слишком много флогов -o"));
			ofile = 1;
			if (cp [1]) {
				/* -ofile */
				outfile = cp+1;
				while (*++cp);
				--cp;
			} else if (i+1 < argc)
				/* -o file */
				outfile = argv[++i];
			break;
		}
		break;
	default:
		if (infile)
			error (MSG ("too many input files",
				"слишком много входных файлов"));
		infile = argv[i];
		break;
	}

	/* настройка ввода-вывода */

	if (infile && ! freopen (infile, "r", stdin))
		error (MSG ("cannot open %s", "не могу открыть %s"), infile);
	if (! freopen (outfile, "w", stdout))
		error (MSG ("cannot open %s", "не могу открыть %s"), outfile);

	if (! fgethdr (stdin, &hdr) || N_BADMAG (hdr))
		error (MSG ("show: %s not an object file\n",
			"show: %s не объектный файл\n"),
			infile ? infile : "stdin");
	size = (hdr.a_const + hdr.a_text + hdr.a_data) / 8;

	word.left = address;
	word.right = size;

	wrword (&word);

	while (--size >= 0) {
		word.right = fgeth (stdin);
		word.left = fgeth (stdin);
		wrword (&word);
	}
	return (0);
}

wrword (w)
register struct word *w;
{
	w->sum = 0;

	if (even (w->left))
		w->sum |= 0x40;
	if (even (w->right))
		w->sum |= 0x20;

	putchar (w->sum);

	puth (w->left);
	puth (w->right);
}

puth (h)
register long h;
{
	putchar ((int) (h >> 24));
	putchar ((int) (h >> 16));
	putchar ((int) (h >> 8));
	putchar ((int) h);
}

fgethdr (text, h)
register FILE *text;
register struct exec *h;
{
	h->a_magic = fgeth (text);      fgeth (text);
	h->a_const = fgeth (text);      fgeth (text);
	h->a_text  = fgeth (text);      fgeth (text);
	h->a_data  = fgeth (text);      fgeth (text);
	h->a_bss   = fgeth (text);      fgeth (text);
	h->a_abss  = fgeth (text);      fgeth (text);
	h->a_syms  = fgeth (text);      fgeth (text);
	h->a_entry = fgeth (text);      fgeth (text);
	h->a_flag  = fgeth (text);      fgeth (text);
	return (1);
}

even (h)
register long h;
{
	register i, odd;

	for (odd=i=0; i<32; ++i)
		odd ^= h >> i;
	return (! (odd & 1));
}
