#include <sys/time.h>
#include <glob.h>
#include "global.h"
#include "robot.h"
#include "maze.h"

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2004 Serge Vakulenko";

char *progname;
int verbose;
int trace;
int debug;

extern char *optarg;
extern int optind;

struct robotab {
	char filename [32];
	int rating;
};

int maxstep = 1000;

int maxmaze = 20;
int nmaze;
maze_t **maze;

int maxrobot = 1000;
int nrobot;
int last_robot_num;
struct robotab *robotab;

int generation;
int smarts;

void usage ()
{
	fprintf (stderr, "Robot Maze Runner, Version %s\n%s\n", version, copyright);
	fprintf (stderr, "Usage:\n\t%s [-vtd] [-s seed] [-m mazes] [-r size]\n", progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	fprintf (stderr, "\t-t\ttrace mode\n");
	fprintf (stderr, "\t-d\tdebug\n");
	fprintf (stderr, "\t-s #\tseed for random generator (default random)\n");
	fprintf (stderr, "\t-m #\tnumber of mazes (default 20)\n");
	fprintf (stderr, "\t-r #\tuse random mazes of given size (default load *.maze)\n");
	exit (-1);
}

/*
 * Цикл движения.
 * Возвращаем:
 * 1  - если робот шагнул.
 * 0  - если остался на месте.
 * -1 - если налетел на стенку
 * -2 - если сделал ход по диагонали
 */
int one_step (robot_t *r, maze_t *m)
{
	/* "Смотрим". */
	r->neuron[0].state = maze_left (m, r->x, r->y) ? 1 : -1;
	r->neuron[1].state = maze_down (m, r->x, r->y) ? 1 : -1;
	r->neuron[2].state = maze_right (m, r->x, r->y) ? 1 : -1;
	r->neuron[3].state = maze_up (m, r->x, r->y) ? 1 : -1;

	/* "Думаем" */
	robot_think (r, 10);
	if (r->neuron[4].state > 0) {
		if (r->neuron[5].state > 0 ||
		    r->neuron[6].state > 0 ||
		    r->neuron[7].state > 0)
			return -2;

		/* Идем вправо. */
		if (maze_right (m, r->x, r->y))
			return -1;
		r->x += 1;
		return 1;
	}
	if (r->neuron[5].state > 0) {
		if (r->neuron[6].state > 0 ||
		    r->neuron[7].state > 0)
			return -2;

		/* Идем вверх. */
		if (maze_up (m, r->x, r->y))
			return -1;
		r->y += 1;
		return 1;
	}
	if (r->neuron[6].state > 0) {
		if (r->neuron[7].state > 0)
			return -2;

		/* Идем влево. */
		if (maze_left (m, r->x, r->y))
			return -1;
		r->x -= 1;
		return 1;
	}
	if (r->neuron[7].state > 0) {
		/* Идем вниз. */
		if (maze_down (m, r->x, r->y))
			return -1;
		r->y -= 1;
		return 1;
	}
	/* Стоим на месте. */
	return 0;
}

int run (robot_t *r, maze_t *m)
{
	int step, idle, result;

	/* Ставим робота в центр лабиринта. */
	robot_setup (r, m->size/2, m->size/2);

	/* Циклы. */
	idle = 0;
	for (step=0; step<maxstep; ++step) {
		result = one_step (r, m);
		if (result < 0) {
			/* Неверный шаг. */
			if (debug)
				printf ("%d - crash\n", step);
			return 0;
		}
		if (result > 0) {
			idle = 0;
			if (r->x < 0 || r->y < 0 ||
			    r->x >= m->size || r->y >= m->size) {
				/* Вышел! */
				if (verbose)
					printf ("%d - exit\n", step);
				return 1;
			}
			if (verbose)
				printf ("%d - %d %d\n", step, r->x, r->y);
			continue;
		}
		++idle;
		if (idle >= 10) {
			/* Остановился. */
			if (verbose)
				printf ("%d - idle\n", step);
			return 0;
		}
	}
	/* Зациклился. */
	if (verbose)
		printf ("%d - abort\n", step);
	return 0;
}

int compute_rating (char *filename)
{
	FILE *fd;
	robot_t *r;
	int rating, i;

	fd = fopen (filename, "r");
	if (! fd) {
		perror (filename);
		exit (1);
	}
	r = robot_load (fd);
	fclose (fd);

	rating = 0;
	for (i=0; i<nmaze; ++i) {
		if (run (r, maze[i]))
			++rating;
	}
	robot_free (r);
	return rating;
}

maze_t *maze_load_file (char *filename)
{
	FILE *fd;
	maze_t *m;

	fd = fopen (filename, "r");
	if (! fd) {
		perror (filename);
		exit (1);
	}
	m = maze_load (fd);
	fclose (fd);
	return m;
}

/*
 * Поиск свободного места в таблице роботов.
 * Если места нет - самый старый и слабый робот удаляется.
 */
int find_slot ()
{
	int last, min_rating, i;

	if (nrobot < maxrobot)
		return nrobot++;

	/* Нет места - ищем самого старого и слабого робота. */
	last = 0;
	min_rating = robotab[last].rating;
	for (i=1; i<nrobot; ++i) {
		if (min_rating > robotab[i].rating) {
			last = i;
			min_rating = robotab[i].rating;
		}
	}
	unlink (robotab[last].filename);

	/* Сдвигаем остаток массива. */
	if (last < nrobot-1)
		memmove (robotab + last, robotab + last + 1,
			sizeof(robotab[0]) * (nrobot - last - 1));
	return nrobot-1;
}

/*
 * Добавление нового робота в таблицу.
 * В качестве имени файла используется следующий свободный номер.
 */
void add_robot (robot_t *r, int rating)
{
	FILE *fd;
	int i;
	static int count;

	i = find_slot ();

	if (rating > robotab[i].rating) {
		/* Если новый робой умнее старого - печатаем "точку". */
		printf (".");
		++smarts;
	} else {
		putchar ("-/|\\" [count++&3]);
		putchar ('\b');
	}
	fflush (stdout);

	sprintf (robotab[i].filename, "%d.robot", ++last_robot_num);
	robotab[i].rating = rating;
	fd = fopen (robotab[i].filename, "w");
	if (! fd) {
		perror (robotab[i].filename);
		exit (1);
	}
	robot_save (r, fd, rating);
	fclose (fd);

	++generation;
}

/*
 * Мутация.
 * Выбираем одного из роботов, вносим мутации и прогоняем
 * через лабиринт. Самых живучих мутантов добавляем в таблицу,
 * возможно, вытесняя старых. Удаляем файлы вытесненных особей.
 */
void mutate ()
{
	FILE *fd;
	robot_t *r, *r_original;
	int rating, old_rating, i, neuron, synapse, neurolink;

	i = random() % nrobot;
	old_rating = robotab[i].rating;

	fd = fopen (robotab[i].filename, "r");
	if (! fd) {
		perror (robotab[i].filename);
		exit (1);
	}
	r = robot_load (fd);
	fclose (fd);

	r_original = 0;
	for (;;) {
		/* Мутация. */
		neuron = 4 + (random() % (r->nneurons - 4));
		assert (neuron >= 4);
		assert (neuron < r->nneurons);

		synapse = random() % r->neuron[neuron].nsynapses;
		assert (synapse >= 0);
		assert (synapse < r->neuron[neuron].nsynapses);

		switch (random() % 7) {
		case 0: default:
			/* Увеличение веса синапса в 2 раза */
			r->neuron[neuron].synapse[synapse].factor *= 2;
			break;
		case 1:
			/* Уменьшение веса синапса в 2 раза */
			r->neuron[neuron].synapse[synapse].factor /= 2;
			break;
		case 2:
			/* Увеличение веса синапса на 1 */
			r->neuron[neuron].synapse[synapse].factor +=
				random() % 1000;
			break;
		case 3:
			/* Уменьшение веса синапса на 1 */
			r->neuron[neuron].synapse[synapse].factor -=
				random() % 1000;
			break;
		case 4:
			/* Изменение знака синапса */
			r->neuron[neuron].synapse[synapse].factor *= -1;
			break;
		case 5:
			/* Случайное значение синапса */
			r->neuron[neuron].synapse[synapse].factor =
				random() % 10000;
			break;
		case 6:
			/* Переставление синапса на другой нейрон */
			neurolink = random() % r->nneurons;
			assert (neurolink >= 0);
			assert (neurolink < r->nneurons);
			r->neuron[neuron].synapse[synapse].neuron =
				r->neuron + neurolink;
			break;
		}

		rating = 0;
		for (i=0; i<nmaze; ++i) {
			if (run (r, maze[i]))
				++rating;
		}
		if (rating < old_rating) {
			/* Робот деградировал. */
			if (r_original) {
				/* Сохраняем немутировавшую копию. */
				add_robot (r_original, old_rating);
			}
			break;
		}
		if (rating > old_rating) {
			add_robot (r, rating);
/*			printf ("New %s rating %d\n", robotab[i].filename, rating);*/
			printf ("%d ", rating);
			fflush (stdout);
			break;
		}

		if (r_original)
			robot_free (r_original);
		r_original = robot_copy (r);
	}
	if (r_original)
		robot_free (r_original);
	robot_free (r);
}

/*
 * Скрещивание.
 * Выбираем пару роботов, скрещиваем разными способами,
 * и выбираем самых способных мутантов.
 */
void cross (int maxcount)
{
	FILE *fd;
	robot_t *r, *r2, *r_original;
	int rating, old_rating, i, i2, n, count;

	i = random() % nrobot;
	do {
		i2 = random() % nrobot;
	} while (i2 == i);

	old_rating = robotab[i].rating;

	fd = fopen (robotab[i].filename, "r");
	if (! fd) {
		perror (robotab[i].filename);
		exit (1);
	}
	r = robot_load (fd);
	fclose (fd);

	fd = fopen (robotab[i2].filename, "r");
	if (! fd) {
		perror (robotab[i2].filename);
		exit (1);
	}
	r2 = robot_load (fd);
	fclose (fd);

	r_original = 0;
	for (count=0; count<maxcount; ++count) {
		/* Скрещивание. */
		n = 4 + (random() % (r->nneurons - 4));
		assert (n >= 4);
		assert (n < r->nneurons);
		assert (r->neuron[n].nsynapses == r2->neuron[n].nsynapses);

		switch (random() % 2) {
		case 0: default:
			/* Берем случайный нейрон от второго робота,
			 * и пришиваем его первому. */
			for (i=r->neuron[n].nsynapses; i<r->neuron[n].nsynapses; ++i) {
				r->neuron[n].synapse[i].factor =
					r2->neuron[n].synapse[i].factor;
				r->neuron[n].synapse[i].neuron = r->neuron +
					(r2->neuron[n].synapse[i].neuron - r2->neuron);
			}
			break;
		case 1:
			/* Берем только один синапс. */
			i = random() % r->neuron[n].nsynapses;
			r->neuron[n].synapse[i].factor =
				r2->neuron[n].synapse[i].factor;
			r->neuron[n].synapse[i].neuron = r->neuron +
				(r2->neuron[n].synapse[i].neuron - r2->neuron);
			break;
		}

		rating = 0;
		for (i=0; i<nmaze; ++i) {
			if (run (r, maze[i]))
				++rating;
		}
		if (rating < old_rating) {
			/* Робот деградировал. */
			if (r_original) {
				/* Сохраняем немутировавшую копию. */
				add_robot (r_original, old_rating);
			}
			break;
		}
		if (rating > old_rating) {
			/* Робот поумнел. */
			add_robot (r, rating);
			printf ("<%d> ", rating);
			fflush (stdout);
			break;
		}

		if (r_original)
			robot_free (r_original);
		r_original = robot_copy (r);
	}
	if (r_original)
		robot_free (r_original);
	robot_free (r);
	robot_free (r2);
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
maze_t *generate (int size, int maxtry, int filenum)
{
	maze_t *m;
	int x, y, ok, fault_counter, color;
	FILE *fd;
	char filename [80];

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

	/* Сохраняем лабиринт в файле N.maze.*/
	sprintf (filename, "%d.maze", filenum);
	fd = fopen (filename, "w");
	if (! fd) {
		perror (filename);
		exit (1);
	}
	maze_save (m, fd);
	fclose (fd);
	return m;
}

int main (int argc, char **argv)
{
	glob_t globbuf;
	int mazesize = 0, i, num;
	unsigned int seed;
	struct timeval tv;

	progname = "robot-tour";
	gettimeofday (&tv, 0);
	seed = tv.tv_sec + tv.tv_usec;
	for (;;) {
		switch (getopt (argc, argv, "vtds:m:r:")) {
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
		case 'm':
			maxmaze = strtol (optarg, 0, 0);
			continue;
		case 'r':
			mazesize = strtol (optarg, 0, 0);
			if (mazesize % 2 != 1) {
				fprintf (stderr, "%s: invalid maze size %d, must be odd\n",
					progname, mazesize);
				exit (1);
			}
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (argc != 0)
		usage ();

	srandom (seed);

	if (mazesize) {
		/* Генерируем случайные лабиринты. */
		nmaze = maxmaze;
		maze = calloc (maxmaze, sizeof (maze_t*));
		for (i=0; i<nmaze; ++i)
			maze[i] = generate (mazesize, 100, i);
		printf ("%d mazes generated.\n", nmaze);
	} else {
		/* Сканируем файлы *.maze и загружаем лабиринты,
		 * на которых будем тренировать роботов. */
		glob ("*.maze", 0, 0, &globbuf);
		nmaze = globbuf.gl_pathc;
		if (nmaze > maxmaze)
			nmaze = maxmaze;
		if (nmaze < 1) {
			fprintf (stderr, "No mazes found\n");
			exit (-1);
		}
		maze = calloc (maxmaze, sizeof (maze_t*));
		assert (maze != 0);
		for (i=0; i<nmaze; ++i)
			maze[i] = maze_load_file (globbuf.gl_pathv[i]);
		globfree (&globbuf);
		printf ("%d mazes loaded.\n", nmaze);
	}
	/* Сканируем файлы *.robot и составляем таблицу роботов. */
	if (glob ("*.robot", 0, 0, &globbuf) != 0) {
		fprintf (stderr, "*.robot: glob error\n");
		exit (-1);
	}
	nrobot = globbuf.gl_pathc;
	if (nrobot > maxrobot)
		nrobot = maxrobot;
	robotab = calloc (maxrobot, sizeof (struct robotab));
	assert (robotab != 0);
	for (i=0; i<nrobot; ++i) {
		strncpy (robotab[i].filename, globbuf.gl_pathv[i],
			sizeof (robotab[i].filename));
		num = strtol (robotab[i].filename, 0, 0);
		if (last_robot_num < num)
			last_robot_num = num;
	}
	globfree (&globbuf);
	printf ("%d robots found (last num %d).\n", nrobot, last_robot_num);

	/* Прогоняем всех роботов через лабиринты,
	 * чтобы составить рейтинг. */
	printf ("Ratings:");
	for (i=0; i<nrobot; ++i) {
		robotab[i].rating = compute_rating (robotab[i].filename);
		printf (" %d", robotab[i].rating);
	}
	printf ("\n");

	for (;;) {
		/* Мутация. */
		mutate ();

		/* Скрещивание. */
		cross (100);

		/* Пауза. */
/*		printf (".");*/
/*		fflush (stdout);*/
/*		sleep (1);*/

		if (generation >= maxrobot) {
			generation = 0;
			if (smarts == 0) {
				/* Если за кучу поколений не было ни одного
				 * поумневшего робота - пора менять лабиринты.
				 * Генерим новый случайный лабиринт. */
				i = random() % nmaze;
				mazesize = maze[i]->size;
				maze_free (maze[i]);
				maze[i] = generate (mazesize, 100, i);
				for (i=0; i<nrobot; ++i) {
					robotab[i].rating = compute_rating (robotab[i].filename);
				}
				printf ("\n[New maze - rating %d]", robotab[0].rating);
			} else {
				smarts = 0;
				printf ("\n[Rating %d]", robotab[0].rating);
			}
		}
	}
	return 0;
}
