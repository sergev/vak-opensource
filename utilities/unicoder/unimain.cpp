/*
 * Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is part of Unicoder utility, which is distributed
 * under the terms of the GNU General Public License (GPL).
 * See the accompanying file "COPYING" for more details.
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <Unicoder.h>

Unicoder input_decoder;
Unicoder output_encoder;

void usage ()
{
	fprintf (stderr, "Unicode Text Converter, Version 1.0\n");
	fprintf (stderr, "Copyright (C) 1999 Serge Vakulenko\n\n");
	fprintf (stderr, "Usage:\n\tunicoder [-I input-format] [-O output-format] [infile [outfile]]\n");
	fprintf (stderr, "Formats:\n");
	fprintf (stderr, "\tiso8859-1 iso8859-2 iso8859-3 iso8859-4 iso8859-5\n");
	fprintf (stderr, "\tiso8859-6 iso8859-7 iso8859-8 iso8859-9 iso8859-14\n");
	fprintf (stderr, "\tiso8859-15 cp037 cp437 cp500 cp737 cp775 cp850 cp852\n");
	fprintf (stderr, "\tcp855 cp857 cp860 cp861 cp862 cp863 cp864 cp865\n");
	fprintf (stderr, "\tcp866 cp869 cp874 cp875 cp1026 cp1250 cp1251 cp1252\n");
	fprintf (stderr, "\tcp1253 cp1254 cp1255 cp1256 cp1257 cp1258 cp10000\n");
	fprintf (stderr, "\tcp10006 cp10007 cp10029 cp10079 cp10081 koi8-r koi8-u\n");
	fprintf (stderr, "\tmac-arabic mac-centeuro mac-croatian mac-cyrillic\n");
	fprintf (stderr, "\tmac-farsi mac-greek mac-iceland mac-roman mac-turkish\n");
	fprintf (stderr, "\tmac-ukraine nextstep usc-2 utf-8\n");
	fprintf (stderr, "Default:\n\tutf-8\n");
	exit (-1);
}

int main (int argc, char **argv)
{
	int c;

	for (;;) {
		switch (getopt (argc, argv, "I:O:")) {
		case EOF:
			break;
		case 'I':
			if (! input_decoder.set_format (optarg)) {
				fprintf (stderr, "Invalid format: %s\n",
					optarg);
				exit (-1);
			}
			continue;
		case 'O':
			if (! output_encoder.set_format (optarg)) {
				fprintf (stderr, "Invalid format: %s\n",
					optarg);
				exit (-1);
			}
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (argc > 2 || (argc == 0 && isatty (0)))
		usage ();
        if (argc > 0 && strcmp (argv[0], "-") != 0 &&
	    ! freopen (argv[0], "r", stdin)) {
		perror (argv[0]);
		exit (-1);
	}
        if (argc == 2 && strcmp (argv[1], "-") != 0 &&
	    ! freopen (argv[1], "w", stdout)) {
		perror (argv[1]);
		exit (-1);
	}

	while ((c = input_decoder.get_char (stdin)) >= 0)
		output_encoder.put_char (c, stdout);
	return 0;
}
