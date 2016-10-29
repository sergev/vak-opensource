/*
 * Тип "стенка".
 * Если 0 - стенки нет.
 * Иначе - номер "цвета" стенки.
 * Связные группы стенок имеют одинаковый цвет.
 */
typedef int border_t;

typedef struct _maze_t {
	int		size;
	border_t	*vertical_bar;
	border_t	horizontal_bar [1];
} maze_t;

maze_t *maze_init (int size);
maze_t *maze_load (FILE *fd);
void maze_save (maze_t *m, FILE *fd);
void maze_free (maze_t *m);

border_t maze_left (maze_t *m, int x, int y);
border_t maze_right (maze_t *m, int x, int y);
border_t maze_up (maze_t *m, int x, int y);
border_t maze_down (maze_t *m, int x, int y);

void maze_set_left (maze_t *m, int x, int y, border_t val);
void maze_set_right (maze_t *m, int x, int y, border_t val);
void maze_set_up (maze_t *m, int x, int y, border_t val);
void maze_set_down (maze_t *m, int x, int y, border_t val);

void maze_recolor (maze_t *m, int old_color, int new_color);
border_t maze_cross_color (maze_t *m, int x, int y);
