/*
 * Эмуляция шины Manchester/RS-485.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#include <string.h>
#include <stdlib.h>
#include "bus.h"

#ifdef MATLAB_MEX_FILE
#   include "mex.h"
#else
#   include "simulator.h"
#endif

/*
 * Выполнение одного шага.
 * Перед вызовом должны быть установлены значения входных портов.
 * После вызова будут установлены значения выходных портов.
 */
void bus_step (bus_t *c)
{
	/*printf ("--%s-- step\n", c->name);*/
	switch (c->run) {
		/* Рабочий режим. Транслируем сигнал с активного входного
		 * порта на выходной порт. */
	case 1:
		c->tx = c->rn0;
		break;
	case 2:
		c->tx = c->rn1;
		break;
	case 4:
		c->tx = c->rn2;
		break;
	case 8:
		c->tx = c->rn3;
		break;
	default:
		/* Холостой режим. Дожидаемся активности на одном из портов
		 * переходим в рабочий режим. */
		if ((! c->rn0) + (! c->rn1) + (! c->rn2) + (! c->rn3) > 1) {
			printf ("--%s-- error: activity %d-%d-%d-%d\n", c->name,
				! c->rn0, ! c->rn1, ! c->rn2, ! c->rn3);
		}
		if (c->rn0 == 0) {
/*			printf ("--%s-- port 0 active\n", c->name);*/
			c->run = 1;
			c->tx = 0;
		} else if (c->rn1 == 0) {
/*			printf ("--%s-- port 1 active\n", c->name);*/
			c->run = 2;
			c->tx = 0;
		} else if (c->rn2 == 0) {
/*			printf ("--%s-- port 2 active\n", c->name);*/
			c->run = 4;
			c->tx = 0;
		} else if (c->rn3 == 0) {
/*			printf ("--%s-- port 3 active\n", c->name);*/
			c->run = 8;
			c->tx = 0;
		} else
			c->tx = 1;

		c->idle_counter = 0;
		break;
	}
	if (c->run) {
		/* Если в течение 4-х циклов активный порт находится
		 * в состоянии "все единицы", переходим в холостой режим. */
		if (c->tx) {
			++c->idle_counter;
			if (c->idle_counter > 4)
				c->run = 0;
		} else
			c->idle_counter = 0;
	}
}

void bus_reset (bus_t *c)
{
	const char *name;

	/*printf ("--%s-- reset\n", c->name);*/
	name = c->name;
	memset (c, 0, sizeof (bus_t));
	c->name = name;
}

bus_t *bus_alloc ()
{
	bus_t *c;

	/*printf ("\n--%s-- started\n", c->name);*/
	c = calloc (1, sizeof (*c));
	return c;
}

void bus_free (bus_t *c)
{
	/*printf ("--%s-- terminated\n\n", c->name);*/
	free (c);
}
