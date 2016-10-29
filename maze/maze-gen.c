#include <sys/time.h>
#include "global.h"
#include "maze.h"

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2004 Serge Vakulenko";

char *progname;
int verbose;
int trace;
int debug;

extern char *optarg;
extern int optind;

void usage ()
{
	fprintf (stderr, "Maze Generator, Version %s\n%s\n", version, copyright);
	fprintf (stderr, "Usage:\n\t%s [-vtd] [-s seed] size\n", progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	fprintf (stderr, "\t-t\ttrace mode\n");
	fprintf (stderr, "\t-d\tdebug\n");
	fprintf (stderr, "\t-s #\tseed for random generator (default random)\n");
	fprintf (stderr, "\tsize\tmaze size (must be odd)\n");
	exit (-1);
}

/*
 * Добавление внутренней стенки к лабиринту.
 * Возвращаем 1, если удалось.
 */
int add_border (maze_t *m, int color)
{
	int x, y, c1, c2, direction;

	/* Выбираем направление */
	direction = random() % 2;

	/* Теперь выбираем случайную клетку,
	 * и выясняем цвета границ по концам. */
	switch (direction) {
	default:
	case 0:	/* Влево */
		x = 1 + random() % (m->size - 1);
		y = random() % m->size;
		if (maze_left (m, x, y))
			return 0;
		c1 = maze_cross_color (m, x, y);
		c2 = maze_cross_color (m, x, y+1);
		break;

	case 1:	/* Вниз */
		x = random() % m->size;
		y = 1 + random() % (m->size - 1);
		if (maze_down (m, x, y))
			return 0;
		c1 = maze_cross_color (m, x, y);
		c2 = maze_cross_color (m, x+1, y);
		break;
	}

	/*
	 * Совмещаем цвета контуров.
	 * Возможно, потребуется перекраска.
	 * Или контуры могут оказаться несовместимыми.
	 */
	if (c1 != 0) {
		if (c2 != 0) {
			/* Оба конца непусты. */
			if (c1 == c2) {
				/* Нельзя замыкать контур. */
				return 0;
			}
			if (c1 > c2) {
				/* Надо перекрасить контур 1. */
				maze_recolor (m, c1, c2);
				color = c2;
			} else {
				/* Надо перекрасить контур 2. */
				maze_recolor (m, c2, c1);
				color = c1;
			}
		} else {
			/* Конец 2 пуст. */
			color = c1;
		}
	} else {
		if (c2 != 0) {
			/* Конец 1 пуст. */
			color = c2;
		}
	}

	/* Ставим новую стенку. */
	switch (direction) {
	default:
	case 0:	/* Влево */
		maze_set_left (m, x, y, color);
		break;

	case 1:	/* Вниз */
		maze_set_down (m, x, y, color);
		break;
	}
	return color;
}

/*
 * Создание лабиринта.
 * Печать на стандартный вывод.
 */
void generate (int size, int maxtry)
{
	maze_t *m;
	int x, y, ok, fault_counter, color;

	m = maze_init (size);

	/* Наружные границы. */
	color = 1;
	for (x=0; x<m->size; ++x) {
		maze_set_down (m, x, 0, 1);
		maze_set_up (m, x, m->size-1, color);
	}
	for (y=0; y<m->size; ++y) {
		maze_set_left (m, 0, y, 1);
		maze_set_right (m, m->size-1, y, color);
	}

	/* Внутренние стенки.
	 * Заканчиваем, когда с N раз не получится. */
	++color;
	for (fault_counter=0; fault_counter<maxtry; ++fault_counter) {
		ok = add_border (m, color);
		if (! ok)
			continue;

		/* Получилось. */
		fault_counter = 0;

		/* Цвет использован? */
		if (ok == color)
			++color;
/*printf ("%d (next %d)\n", ok, color);*/
/*maze_save (m, stdout);*/
	}

	/* Выход. */
	switch (random() % 4) {
	case 0:
		maze_set_left (m, 0, random() % m->size, 0);
		break;
	case 1:
		maze_set_right (m, m->size-1, random() % m->size, 0);
		break;
	case 2:
		maze_set_down (m, random() % m->size, 0, 0);
		break;
	case 3:
		maze_set_up (m, random() % m->size, m->size-1, 0);
		break;
	}

	maze_save (m, stdout);
}

int main (int argc, char **argv)
{
	int size;
	unsigned int seed;
	struct timeval tv;

	progname = "maze-gen";
	gettimeofday (&tv, 0);
	seed = tv.tv_sec + tv.tv_usec;
	for (;;) {
		switch (getopt (argc, argv, "vtds:")) {
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
		case 's':
			seed = strtol (optarg, 0, 0);
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (argc != 1)
		usage ();

	size = strtol (argv[0], 0, 0);
	if (size % 2 != 1) {
		fprintf (stderr, "%s: invalid size %d, must be odd\n",
			progname, size);
		exit (1);
	}
	srandom (seed);
	generate (size, 100);
	return (0);
}
