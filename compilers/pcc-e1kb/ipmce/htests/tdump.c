# include <stdio.h>

# define WORDSZ 9

int skip = 0;
int eof = 0;

main (argc, argv)
char **argv;
{
	int infile = 0;
	register i, c, addr;

	for (i=1; i<argc; ++i)
		if (argv [i] [0] == '-')
			skip = atoi (& argv [i] [1]);
		else if (! infile) {
			if (! freopen (argv [i], "r", stdin)) {
				fprintf (stderr, "cannot open %s\n", argv [i]);
				return (1);
			}
			infile = 1;
		} else {
			fprintf (stderr, "too many input files\n");
			return (1);
		}
	while (--skip >= 0)
		getchar ();
	for (addr=0; ! eof; ++addr) {
		printf ("%05xh\t", addr);

		prbyte (getchar ());

		putchar (' ');

		prbyte (getchar ());
		prbyte (getchar ());
		prbyte (getchar ());
		prbyte (getchar ());

		putchar (' ');

		prbyte (getchar ());
		prbyte (getchar ());
		prbyte (getchar ());
		prbyte (getchar ());

		putchar ('\n');
	}
	return (0);
}

prbyte (c)
register c;
{
	if (c < 0)
		eof = 1;
	else
		printf ("%02x", c);
}
