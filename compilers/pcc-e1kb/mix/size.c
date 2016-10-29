/*
 *      ОС ДЕМОС СВС-Б.
 *
 *      size [-w] [file ...]    - выдать размеры сегментов объектного файла.
 *                                Если задан флаг "-w", размеры выдаются
 *                                в словах, иначе - в байтах.
 *
 *      Автор: Вакуленко С.В. (МФТИ).
 */

# include <stdio.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

# define W 8            /* длина слова в байтах */

int header;             /* был ли уже напечатан заголовок */
int wflag;              /* выдавать длину в словах */

# define MSG(l,r) (msg ? (r) : (l))

char msg;

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
			while (*++*argv) switch (**argv) {
			case 'w':
				wflag++;
				break;
			default:
				fprintf (stderr,
					MSG ("size: bad flag %c\n",
						"size: неизвестный флаг %c\n"),
					**argv);
				exit (1);
			}
			continue;
		}
		size (*argv);
		yesarg = 1;
	}
	if (! yesarg) size ("a.out");
}

size (fname)
register char *fname;
{
	struct exec buf;
	long sum;
	register FILE *f;

	if ((f = fopen(fname, "r"))==NULL) {
		printf (MSG ("size: %s not found\n",
				"size: %s не найден\n"),
			fname);
		return;
	}
	if (! fgethdr (f, &buf) || N_BADMAG(buf)) {
		printf (MSG ("size: %s not an object file\n",
				"size: %s не объектный файл\n"),
			fname);
		fclose(f);
		return;
	}
	if (header == 0) {
		printf("const\ttext\tdata\tbss\tabss\tdec\thex\n");
		header = 1;
	}
	sum = buf.a_const + buf.a_text + buf.a_data + buf.a_bss + buf.a_abss;
	if (wflag) {
		sum /= W;
		printf("%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%lx\t%s\n",
			buf.a_const/W, buf.a_text/W,
			buf.a_data/W, buf.a_bss/W, buf.a_abss/W,
			sum, sum, fname);
	} else {
		printf("%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%lx\t%s\n",
			buf.a_const, buf.a_text,
			buf.a_data, buf.a_bss, buf.a_abss,
			sum, sum, fname);
	}
	fclose(f);
}
