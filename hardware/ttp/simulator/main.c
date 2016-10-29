/*
 * Интерфейс командной строки для эмулятора кластера TTP.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "simulator.h"
#include "simstruct.h"

/*
 * Выдача текста в журнал событий.
 * Используется вместо printf().
 */
void ui_print (const char *fmt, ...)
{
	va_list ap;

	va_start (ap, fmt);
	vprintf (fmt, ap);
	va_end (ap);
}

/*
 * Подкрашиваем активный блок.
 */
void ui_draw_ctlr (simulator_t *sim, int i, int active)
{
}

void ui_draw_cpu (simulator_t *sim, int i, int active)
{
}

int main (int argc, char *argv[])
{
	simulator_t *sim;

	sim = calloc (1, sizeof (*sim));
	if (! sim) {
		fprintf (stderr, "Out of memory\n");
		exit (1);
	}
	simulator_init (sim);

	/* Установка параметров модели. */
	simulator_set_bus_rate (sim, 10);			/* 10 Мбит/сек */

	simulator_set_cpu_frequency (sim, 0, 10);		/* 50 МГц */
	simulator_set_ctlr_frequency (sim, 0, 0);		/* +100 ppm */

	simulator_set_cpu_frequency (sim, 1, 10);		/* 20 МГц */
	simulator_set_ctlr_frequency (sim, 1, 0);		/* -100 ppm */
#if NNODES > 2
	simulator_set_cpu_frequency (sim, 2, 50);		/* 50 МГц */
	simulator_set_ctlr_frequency (sim, 2, 0);		/* 0 ppm */
#endif
#if NNODES > 3
	simulator_set_cpu_frequency (sim, 3, 50);		/* 50 МГц */
	simulator_set_ctlr_frequency (sim, 3, 0);		/* 0 ppm */
#endif
	printf ("Started.\n");

	/* Пуск симулятора в непрерывном режиме. */
	for (;;) {
		simulator_step (sim);
/*		printf ("Шаг %d, время %.5f мксек\n", sim->nstep, sim->time_usec);*/
/*		usleep (10000);*/
	}
	return 0;
}
