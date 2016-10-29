/*
 *      Usage
 *              metaplot [-e] [-j] [-m] file...
 *      or
 *              metaprint [-e] [-j] file...
 *
 *      Description
 *              Plot metafile to graphics defice.
 *
 *      Options
 *              -r      - reverse axes
 *              -e      - plot to stdout in epson-80 format,
 *                      75 dots per inch, default for metaprint
 *              -j      - plot to stdout in laserjet format,
 *                      150 dots per inch
 *              -m      - plot to stdout in metafile format
 */

# include <stdio.h>

static device = 1;              /* device for plotting */

main (argc, argv)
char **argv;
{
	int inflag = 0;

	scanname (*argv);
	while (*++argv) {
		if (**argv == '-')
			flags (*argv + 1);
		else {
			inflag = 1;
			if (! freopen (*argv, "r", stdin))
				fprintf (stderr, "metaplot: cannot open %s\n", *argv);
			else
				draw ();
		}
	}
	if (! inflag)
		draw ();
	return (0);
}

static scanname (p)
register char *p;
{
	register char *s;

	/* find last file name */
	for (s=p; *p; ++p)
		if (*p == '/')
			s = p+1;
	if (! strcmp (s, "metaprint"))
		device = 2;
}

static flags (p)
register char *p;
{
	for (; *p; ++p)
		switch (*p) {
		case 'm':
			/* draw to metafile */
			device = 0;
			break;
		case 'e':
			/* epson */
			device = 3;
			break;
		case 'j':
			/* laserjet */
			device = 4;
			break;
		case 'r':
			/* reverse axes */
			GReverse ();
			break;
		dewfault:
			fprintf (stderr, "metaplot: unknown option -%c\n", *p);
			break;
		}
}

draw ()
{
	register c;
	int x, y, n;

	while ((c = getchar ()) != EOF) {
		switch (c) {
		default:
			while ((c = getchar ()) != EOF && c != '\n');
			break;
		case '(':
			scanf ("%d,%d\n", &x, &y);
			GOpen (device, x, y);
			break;
		case ')':
			GClose ();
			while ((c = getchar ()) != EOF && c != '\n');
			break;
		case '!':
			scanf ("%d\n", &n);
			GColor (n);
			break;
		case '=':
			scanf ("%d,%d\n", &x, &y);
			GMove (x, y);
			break;
		case '-':
			scanf ("%d,%d\n", &x, &y);
			GPlot (x, y);
			break;
		case '@':
			scanf ("%d,%d,%d\n", &x, &y, &n);
			GPaint (n, x, y);
			break;
		}
	}
}
