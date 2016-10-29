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
 *              -b      - plot to stdout in BGL format (BENSON)
 *              -h      - plot to stdout in HP-GL format (HP plotters)
 *              -x      - run under XWindows
 */
#include <stdio.h>
#include <stdlib.h>

#ifdef __MSDOS__
#   include <dir.h>
#endif

static device = 1;              /* device for plotting */

char *helptab [] = {
	"Usage:  metaplot [-r] [-m] [-e] [-j] [-b] [-h] file...",
	"or      metaprint [-r] [-e] [-j] [-b] [-h] file...",
	"",
	"Description:",
	"        Plot metafile to graphics defice.",
	"",
	"Options:",
	"        -r  - reverse axes",
	"        -m  - plot to stdout in metafile format",
	"        -e  - plot to stdout in epson-80 format, 75 dpi, default for metaprint",
	"        -j  - plot to stdout in laserjet format, 150 dpi",
	"        -b  - plot to stdout in BGL format (BENSON)",
	"        -h  - plot to stdout in HP-GL format (HP plotters)",
	"        -x  - run under XWindows",
	0,
};

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
		case 'x':
			/* XWindows */
			device = 100;
			break;
		case 'e':
			/* epson */
			device = 3;
			break;
		case 'j':
			/* laserjet */
			device = 4;
			break;
		case 'b':
			/* BGL */
			device = 5;
			break;
		case 'h':
			/* HP-GL */
			device = 6;
			break;
		case 'r':
			/* reverse axes */
			GReverse ();
			break;
		dewfault:
			fprintf (stderr, "metaplot: unknown option -%c, use 'metaplot -help' for help\n", *p);
			break;
		}
}

int main (argc, argv)
char **argv;
{
	int inflag = 0;
	register char *name = 0;

	if (argc == 2 && ! strcmp (argv [1], "-help")) {
		help ();
		exit (0);
	}
	scanname (*argv);
	while (++argv, --argc > 0) {
		if (**argv == '-')
			flags (*argv + 1);
		else {
# ifdef __MSDOS__
			static struct ffblk f;

			for (;;) {
				if (name) {
					if (findnext (&f) < 0) {
						name = 0;
						break;
					}
				} else {
					if (findfirst (*argv, &f, 0) < 0) {
						fprintf (stderr,
							"Cannot find %s\n",
							*argv);
						break;
					} else
						name = f.ff_name;
				}
# else
			name = *argv;
			{
# endif
				inflag = 1;
				if (! freopen (name, "r", stdin))
					fprintf (stderr,
						"metaplot: cannot open %s\n",
						name);
				else
					draw ();
			}
		}
	}
	if (! inflag)
		draw ();
	GExit ();
	return (0);
}

help ()
{
	register char **p;

	for (p=helptab; *p; ++p)
		puts (*p);
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
