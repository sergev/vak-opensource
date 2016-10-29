
# include <stdio.h>

extern long fgeth ();

main (argc, argv)
register char *argv[];
{
	register i;

	if (argc > 1) for (i=1; i<argc; i++) ot (argv[i]);
	else ot ("a.out");
}

ot (name)
char *name;
{
	register long l, r;

	if (freopen (name, "r", stdin) == NULL) {
		fprintf (stderr, "can't open %s\n", name);
		return;
	}
	for (;;) {
		r = fgeth (stdin);
		if (feof (stdin)) return;
		l = fgeth (stdin);
		printf ("%011lo %011lo\n", l, r);
	}
}
