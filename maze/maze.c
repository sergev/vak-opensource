#include "global.h"
#include "maze.h"

maze_t *maze_init (int size)
{
	maze_t *m;

	/* Размер лабиринта должен быть нечетным. */
	assert (size > 0);
	assert (size % 2 == 1);
	m = calloc (1, sizeof(maze_t) + sizeof(border_t) * size * (size+1) * 2);
	assert (m != 0);

	m->size = size;
	m->vertical_bar = m->horizontal_bar + size * (size+1);
	return m;
}

maze_t *maze_load (FILE *fd)
{
	maze_t *m;
	int size, x, y;
	char line [1024], *line_good;

	/* Размер лабиринта должен быть нечетным. */
	line_good = fgets (line, sizeof (line), fd);
	assert (line_good);

	sscanf (line, "maze %d", &size);
	assert (size > 0);
	assert (size % 2 == 1);
	m = calloc (1, sizeof(maze_t) + sizeof(border_t) * size * (size+1) * 2);
	assert (m != 0);

	m->size = size;
	m->vertical_bar = m->horizontal_bar + size * (size+1);

	/* Чтение верхней границы. */
	line_good = fgets (line, sizeof (line), fd);
	assert (line_good);
	for (x=0; x<m->size; ++x) {
		if (line [2*x+1] != ' ')
			maze_set_up (m, x, m->size-1, 1);
		else
			maze_set_up (m, x, m->size-1, 0);
	}

	for (y=m->size-1; y>=0; --y) {
		/* Чтение боков и низов. */
		line_good = fgets (line, sizeof (line), fd);
		assert (line_good);

		for (x=0; x<m->size; ++x) {
			if (line[2*x] != ' ')
				maze_set_left (m, x, y, 1);
			else
				maze_set_left (m, x, y, 0);

			if (line[2*x+1] != ' ')
				maze_set_down (m, x, y, 1);
			else
				maze_set_down (m, x, y, 0);
		}
		if (line[2*m->size] != ' ')
			maze_set_right (m, m->size-1, y, 1);
		else
			maze_set_right (m, m->size-1, y, 0);
	}
	return m;
}

void maze_save (maze_t *m, FILE *fd)
{
	int x, y;

	fprintf (fd, "maze %d\n", m->size);

	/* Печать верхней границы. */
	for (x=0; x<m->size; ++x) {
		if (maze_up (m, x, m->size-1))
			fprintf (fd, " _");
		else
			fprintf (fd, "  ");
	}
	fprintf (fd, "\n");

	for (y=m->size-1; y>=0; --y) {
		/* Печать боков и низов. */
		for (x=0; x<m->size; ++x) {
			if (maze_left (m, x, y))
				fprintf (fd, "|");
			else
				fprintf (fd, " ");

			if (maze_down (m, x, y))
				fprintf (fd, "_");
			else
				fprintf (fd, " ");
		}
		if (maze_right (m, m->size-1, y))
			fprintf (fd, "|");
		else
			fprintf (fd, " ");
		fprintf (fd, "\n");
	}
}

void maze_free (maze_t *m)
{
	free (m);
}

border_t maze_left (maze_t *m, int x, int y)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (x < m->size);
	assert (y < m->size);
	return m->vertical_bar [x * m->size + y];
}

border_t maze_right (maze_t *m, int x, int y)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (x < m->size);
	assert (y < m->size);
	return m->vertical_bar [(x+1) * m->size + y];
}

border_t maze_up (maze_t *m, int x, int y)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (x < m->size);
	assert (y < m->size);
	return m->horizontal_bar [x * (m->size+1) + (y+1)];
}

border_t maze_down (maze_t *m, int x, int y)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (x < m->size);
	assert (y < m->size);
	return m->horizontal_bar [x * (m->size+1) + y];
}

void maze_set_left (maze_t *m, int x, int y, border_t val)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (x < m->size);
	assert (y < m->size);
	m->vertical_bar [x * m->size + y] = val;
}

void maze_set_right (maze_t *m, int x, int y, border_t val)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (x < m->size);
	assert (y < m->size);
	m->vertical_bar [(x+1) * m->size + y] = val;
}

void maze_set_up (maze_t *m, int x, int y, border_t val)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (x < m->size);
	assert (y < m->size);
	m->horizontal_bar [x * (m->size+1) + (y+1)] = val;
}

void maze_set_down (maze_t *m, int x, int y, border_t val)
{
	assert (x >= 0);
	assert (y >= 0);
	assert (x < m->size);
	assert (y < m->size);
	m->horizontal_bar [x * (m->size+1) + y] = val;
}

void maze_recolor (maze_t *m, int old_color, int new_color)
{
	border_t *p, *limit;

	limit = m->horizontal_bar + 2 * m->size * (m->size + 1);
	for (p=m->horizontal_bar; p<limit; ++p)
		if (*p == old_color)
			*p = new_color;
}

/*
 * Выяснение цвета границ, примыкающих к левому нижнему углу
 * заданной клетки. Четыре палочки образуют крестик.
 * Все они должны иметь одинаковый цвет.
 */
border_t maze_cross_color (maze_t *m, int x, int y)
{
	border_t c;

	assert (x >= 0);
	assert (y >= 0);
	assert (x <= m->size);
	assert (y <= m->size);

	/* Верхний луч - слева. */
	c = m->vertical_bar [x * m->size + y];
	if (c != 0)
		return c;

	/* Правый луч - снизу. */
	c = m->horizontal_bar [x * (m->size+1) + y];
	if (c != 0)
		return c;

	/* Нижний луч - ниже слева. */
	if (y > 0) {
		c = m->vertical_bar [x * m->size + (y-1)];
		if (c != 0)
			return c;
	}

	/* Левый луч - левее снизу. */
	if (x > 0) {
		c = m->horizontal_bar [(x-1) * (m->size+1) + y];
		if (c != 0)
			return c;
	}
	return 0;
}
