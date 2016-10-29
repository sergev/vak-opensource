# include <stdio.h>

# if defined (__MSDOS__) && !defined (MSDOS)
#    define MSDOS
# endif

# define FILNAMSZ       128
# define LINESZ         512

FILE *new_file, *old_file;
char tmpbox [FILNAMSZ];
char line [LINESZ];
int linecnt;
int tty;

# ifdef MSDOS
# define FBUFSZ         8192

char old_buf [FBUFSZ];
char new_buf [FBUFSZ];

static unsigned char koi8alt[] = {
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xc8, 0xc9, 0xca, 0xff, 0xcc, 0xcd, 0xce, 0xcf,
	0xd0, 0xd1, 0xd2, 0xf1, 0xd4, 0xd5, 0xd6, 0xd7,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xd3, 0xf3, 0xf2, 0xf0, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xcb,
	0xee, 0xa0, 0xa1, 0xe6, 0xa4, 0xa5, 0xe4, 0xa3,
	0xe5, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae,
	0xaf, 0xef, 0xe0, 0xe1, 0xe2, 0xe3, 0xa6, 0xa2,
	0xec, 0xeb, 0xa7, 0xe8, 0xed, 0xe9, 0xe7, 0xea,
	0x9e, 0x80, 0x81, 0x96, 0x84, 0x85, 0x94, 0x83,
	0x95, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e,
	0x8f, 0x9f, 0x90, 0x91, 0x92, 0x93, 0x86, 0x82,
	0x9c, 0x9b, 0x87, 0x98, 0x9d, 0x99, 0x97, 0x9a,
};
# endif /* MSDOS */

int parse ();
int copy ();
int batched ();
int process ();
int copymail ();
int unbatch ();

extern char *strncpy ();
extern char *strcpy ();
extern int isatty ();
extern int unlink ();
extern int compack ();

int main (argc, argv)
int argc;
char *argv[];
{
	long len;

	if (argc != 2) {
		fprintf (stderr, "Usage: unbatch <filename>\n");
		return (-1);
	}
	old_file = fopen (argv[1], "r");
	if (! old_file) {
		fprintf (stderr, "Cannot read %s\n", argv[1]);
		return (-1);
	}
	tmpnam (tmpbox);
	tty = isatty (1);
	new_file = fopen (tmpbox, "w");
	if (! new_file) {
		fprintf (stderr, "Cannot write to %s\n", tmpbox);
		return (-1);
	}
# ifdef MSDOS
	setvbuf (old_file, old_buf, _IOFBF, FBUFSZ);
	setvbuf (new_file, new_buf, _IOFBF, FBUFSZ);
# else
	setbuf (stdout, (char *) 0);
# endif
	if (parse () < 0) {
		fclose (new_file);
		unlink (tmpbox);
		return (-1);
	}
	fflush (new_file);

	len = ftell (new_file);
	fclose (old_file);
	fclose (new_file);

	printf ("Updating mailbox%s", tty ? " " : "...");
	if (copy (tmpbox, argv[1], len) < 0) {
		printf (" cancelled\n");
		fprintf (stderr, "Cannot copy %s to %s\n", tmpbox, argv[1]);
		fprintf (stderr, "Your mailbox saved to %s\n", tmpbox);
		return (-1);
	}
	printf (" done\n");
	unlink (tmpbox);
	return (0);
}

/*
 * Read mailbox and unpack all batches.
 * Write result to temporary file.
 * Return 0 if ok, -1 on error.
 */
int parse ()
{
	char savfrom [LINESZ];
	int nm, successfull, skipped;
	long pos;

	nm = 0;
	successfull = 0;
	skipped = 0;
	if (! fgets (line, LINESZ, old_file))
		return (-1);
	for (;;) {
		if (strncmp (line, "From ", 5) == 0) {
			++nm;
			pos = ftell (old_file);
			strcpy (savfrom, line);
			printf ("Process message %d ", nm);
			if (batched ()) {
				printf ("unpack%s", tty ? " " : "...");
				if (process () < 0 || ferror (new_file)) {
					if (tty)
						printf (" cancelled\n");
					fprintf (stderr, "Process error\n");
					return (-1);
				}
				++successfull;
				printf (" done\n");
				continue;
			} else {
				printf ("skip%s", tty ? " " : "...");
				/* Copy the first line "From " */
				if (fputs (savfrom, new_file) < 0)
					return (-1);
				if (tty)
					putchar ('.');
				fseek (old_file, pos, 0);
				if (copymail () < 0 || ferror (new_file)) {
					if (tty)
						printf (" cancelled\n");
					fprintf (stderr, "Copy error\n");
					return (-1);
				}
				++skipped;
				printf (" done\n");
				continue;
			}
		}
		if (! fgets (line, LINESZ, old_file))
			break;
	}
	if (skipped) {
		printf ("Total %d messages tansferred, ", successfull);
		printf ("%d messages skipped\n", skipped);
	} else {
		printf ("Total %d messages tansferred\n", successfull);
	}
	return (0);
}

/*
 * Check if current message is valid batch.
 * Batch must contain line "X-Batch: pack" or "X-Batch: compress"
 * in header.
 * Return 1 if batch found, else 0.
 * Always skip all the header.
 */
int batched ()
{
	while (fgets (line, LINESZ, old_file) && line[0]!='\n')
		if (strcmp (line, "X-Batch: pack\n") == 0 ||
		    strcmp (line, "X-Batch: compress\n") == 0) {
			/* skip header */
			while (fgets (line, LINESZ, old_file) &&
				line[0]!='\n');
			return (1);
		}
	return (0);
}

/*
 * Copy current letter to temporary file.
 * Stop just before the next "From ".
 * Return 0 if ok, -1 on error.
 */
int copymail ()
{
	while (fgets (line, LINESZ, old_file) && strncmp (line, "From ", 5)) {
		if (fputs (line, new_file) < 0)
			return (-1);
		if (tty)
			putchar ('.');
	}
	return (0);
}

int get ()
{
	return (getc (old_file));
}

void put (c)
register c;
{
# ifdef MSDOS
	if (c & 0x80)
		c = koi8alt [c & 0x7f];
# endif
	line [linecnt++] = c;
	if (c=='\n' || linecnt >= LINESZ - 1) {
		line [linecnt] = 0;
		if (strncmp (line, "From_", 5) == 0) {
			line [4] = ' ';
			if (tty)
				putchar ('#');
		} else if (strncmp (line, ">From_", 6) == 0) {
			register char *p;

			for (p=line; *p; ++p)
				*p = p[1];
		}
		fputs (line, new_file);
		if (tty)
			putchar ('.');
		linecnt = 0;
	}
}

/*
 * Determine type of batch, then process it.
 * Stop just before the next "From ".
 * Return 0 if ok, -1 on error.
 */
int process ()
{
	int count;
	long pos;

	count = 0;
	for (;;) {
		pos = ftell (old_file);
		if (! fgets (line, LINESZ, old_file)) {
eomsg:                  if (! count)
				printf ("(empty)");
			break;
		}
		if (strncmp (line, "From ", 5) == 0)
			goto eomsg;
		if (strncmp (line, "From_", 5) == 0) {
			++count;
			if (unbatch () < 0)
				return (-1);
		} else if (strncmp (line, "start", 5) == 0) {
			fseek (old_file, pos, 0);
			++count;
			linecnt = 0;
			if (compack (3, get, put) < 0)
				return (-1);
			if (linecnt) {
				if (line [linecnt-1] != '\n')
					line [linecnt++] = '\n';
				line [linecnt] = 0;
				fputs (line, new_file);
			}
		}
		fputs ("\n", new_file);
		if (ferror (new_file))
			return (-1);
	}
	return (0);
}

/*
 * Unbatch current letter to temporary file.
 * Stop just before the next "From ".
 * Return 0 if ok, -1 on error.
 */
int unbatch ()
{
	for (;;) {
		if (strncmp (line, "From ", 5) == 0)
			break;
		if (strncmp (line, "From_", 5) == 0) {
			line [4] = ' ';
			if (tty)
				putchar ('#');
		} else if (strncmp (line, ">From_", 6) == 0) {
			register char *p;

			for (p=line; *p; ++p)
				*p = p[1];
		}
		fputs (line, new_file);
		if (tty)
			putchar ('.');
		if (! fgets (line, LINESZ, old_file))
			break;
	}
	return (0);
}

/*
 * Copy file to file, len bytes.
 * Return 0 on success, -1 on error.
 */
int copy (from, to, len)
char *from, *to;
long len;
{
	register n, k;
	register char *p;
	char buf [512];
	register FILE *ofd, *ifd;
	int rez;

	ifd = fopen (from, "r");
	if (! ifd)
		return (-1);
# ifdef MSDOS
	if (unlink (to) < 0) {
		fclose (ifd);
		return (-1);
	}
# endif
	ofd = fopen (to, "w");
	if (! ofd) {
		fclose (ifd);
		return (-1);
	}
# ifdef MSDOS
	setvbuf (ifd, old_buf, _IOFBF, FBUFSZ);
	setvbuf (ofd, new_buf, _IOFBF, FBUFSZ);
# endif
	rez = 0;
	while (len > 0) {
		n = sizeof (buf);
		if (len < n)
			n = len;
		n = fread (buf, 1, n, ifd);
		if (n < 0) {
			rez = -1;
			break;
		}
		if (n == 0)
			break;
		len -= n;
		p = buf;
		while (n > 0) {
			k = fwrite (p, 1, n, ofd);
			if (k <= 0) {
				rez = -1;
				break;
			}
			n -= k;
			p += k;
			if (tty)
				putchar ('.');
		}
	}
	fflush (ofd);
	fclose (ifd);
	fclose (ofd);
	return (rez);
}
