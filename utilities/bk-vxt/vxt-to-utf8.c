/*
 * Decode BK-0010 VXT file.
 *
 * Copyright (C) 2009 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iconv.h>

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2009 Serge Vakulenko";

char *progname;
int verbose;
int trace;
int debug;
iconv_t conv;

void decode_char (int c)
{
	char inbuf[1], *inptr;
	char outbuf[8], *outptr;
	size_t inbytes, outbytes;

	inbuf[0] = c;
	inptr = inbuf;
	inbytes = 1;
	outptr = outbuf;
	outbytes = sizeof (outbuf);

	if (iconv (conv, &inptr, &inbytes, &outptr, &outbytes) == (size_t) -1) {
		fprintf (stderr, "iconv %03o", c);
		perror ("");
		return;
	}
	if (outbytes < sizeof (outbuf))
		fwrite (outbuf, 1, sizeof (outbuf) - outbytes, stdout);
}

void decode (char *filename)
{
	FILE *fd;
	int c, d;

	fd = fopen (filename, "r");
	for (;;) {
		c = getc (fd);
		switch (c) {
		case EOF:
		case 013:			/* Конец файла */
			fclose (fd);
			return;
		case '\n':
		case 0:				/* Перевод строки. */
			fputc ('\n', stdout);
			break;
		case 1: case 2: case 3: case 4:
		case 5: case 6: case 7: case 8:	/* Пробелы. */
			d = getc (fd);
			if (d >= 1 && d <= 8) {
				c += d * 8;
				d = getc (fd);
				if (d >= 1 && d <= 8) {
					c += d * 8 * 8;
				} else {
					ungetc (d, fd);
				}
			} else {
				ungetc (d, fd);
			}
			while (c-- > 0)
				fputc (' ', stdout);
			break;
		case 020:			/* Отмена выделения */
		case 021:			/* ? */
		case 022:			/* Подчеркивание */
		case 023:			/* Нижний индекс */
		case 024:			/* Жирный шрифт */
		case 026:			/* ? */
		case 030:			/* Курсив */
			break;
		case 0265: fputs ("─", stdout); break;	/* Горизонтальная линия */
		case 0267: fputs ("│", stdout); break;	/* Вертикальная линия */
		case 0252: fputs ("┌", stdout); break;	/* Угол верхний левый */
		case 0243: fputs ("┐", stdout); break;	/* Угол верхний правый */
		case 0246: fputs ("└", stdout); break;	/* Угол нижний левый */
		case 0271: fputs ("┘", stdout); break;	/* Угол нижний правый */
		case 0245: fputs ("├", stdout); break;	/* Т левое */
		case 0260: fputs ("┤", stdout); break;	/* Т правое */
		case 0253: fputs ("┬", stdout); break;	/* Т верхнее */
		case 0241: fputs ("┴", stdout); break;	/* Т нижнее */
		case 0256: fputs ("┼", stdout); break;	/* Перекрестие */
		case 0202: fputs ("*", stdout); break;	/* Перекрестие */

		case 0207: fputs ("━", stdout); break;	/* Горизонтальная линия */
		case 0217: fputs ("┃", stdout); break;	/* Вертикальная линия */
		case 0233: fputs ("┏", stdout); break;	/* Угол верхний левый */
		case 0235: fputs ("┓", stdout); break;	/* Угол верхний правый */
		case 0226: fputs ("┗", stdout); break;	/* Угол нижний левый */
		case 0204: fputs ("┛", stdout); break;	/* Угол нижний правый */
		case 0206: fputs ("┣", stdout); break;	/* Т левое */
		case 0231: fputs ("┫", stdout); break;	/* Т правое */
		case 0232: fputs ("┻", stdout); break;	/* Т нижнее */
		case 0214: fputs ("┳", stdout); break;	/* Т верхнее */
//		case 0256: fputs ("╋", stdout); break;	/* Перекрестие */

		case 0216: fputs ("┨", stdout); break;
		case 0222: fputs ("┠", stdout); break;
		case 0234: fputs ("╂", stdout); break;
		case 0237: fputs ("┿", stdout); break;
		case 0223: fputs ("╟", stdout); break;	/* Заземление */
		case 0247: fputs ("═", stdout); break;
		case 0244: fputs ("╡", stdout); break;
		case 0275: fputs ("╞", stdout); break;
		case 0272: fputs ("╪", stdout); break;
		case 0261: fputs ("←", stdout); break;	/* Стрелки */
		case 0276: fputs ("→", stdout); break;
		case 0263: fputs ("↑", stdout); break;
		case 0255: fputs ("↓", stdout); break;
		case 0215: fputs ("⎕", stdout); break;	/* Резистор вертикально */
		case 0211: fputs ("⊏", stdout); break;	/* Резистор горизонтально */
		case 0224: fputs ("⊐", stdout); break;
		case 0212: fputs ("┾", stdout); break;	/* Контакт слева */
		case 0203: fputs ("┽", stdout); break;	/* Контакт справа */
		case 0221: fputs ("┾", stdout); break;	/* Инверсный контакт слева */
		case 0230: fputs ("╪", stdout); break;	/* Конденсатор */
		case 0210: fputs ("⎕", stdout); break;	/* Кварц */
		case 0200: fputs ("▶", stdout); break;	/* Диод вправо ◀ */
		case 0225: fputs ("⟠", stdout); break;	/* Перечёркнутый ромб */
		case 0227: fputs ("⍚", stdout); break;	/* Подчёркнутый ромб */
		case 0177: fputs ("█", stdout); break;
		case 0251: fputs ("♠", stdout); break;

		case 0205: fputs ("├", stdout); break;	/* Спайка левая */
		case 0213: fputs ("┬", stdout); break;	/* Спайка верхняя */
		case 0220: fputs ("┤", stdout); break;	/* Спайка правая */
		case 0201: fputs ("┴", stdout); break;	/* Спайка нижняя */
		default:
			if (c >= ' ' && c <= '~') {
				/* Символ ASCII. */
				fputc (c, stdout);
			} else if (c >= 0300 && c <= 0377) {
				/* Символ кириллицы. */
				decode_char (c);
			} else if (c > '~') {
				/* Символ псевдографики. */
				printf ("<%03o>", c);
			} else {
				printf ("<%03o>", c);
			}
			break;
		}
	}
}

void usage ()
{
	fprintf (stderr, "Decode VXT to UTF8, Version %s, %s\n", version, copyright);
	fprintf (stderr, "Usage:\n\t%s [-vtd] file...\n", progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	fprintf (stderr, "\t-t\ttrace mode\n");
	fprintf (stderr, "\t-d\tdebug\n");
	exit (-1);
}

int main (int argc, char **argv)
{
	int i;

	progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "vtd")) {
		case EOF:
			break;
		case 'v':
			++verbose;
			continue;
		case 't':
			++trace;
			continue;
		case 'd':
			++debug;
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;
	if (argc < 1)
		usage ();

	conv = iconv_open ("UTF-8", "KOI8-R");
	if (conv == (iconv_t) -1) {
		perror ("iconv from KOI8 to UTF8");
		exit (-1);
	}
	for (i=0; i<argc; ++i)
		decode (argv[i]);
	iconv_close (conv);

	return (0);
}
