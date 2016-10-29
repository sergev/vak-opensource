/*
 * Эмуляция процессора узла #0 кластера TTP.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#include "ttc-reg.h"
#include "cpu.h"
#include "node.h"

#ifdef MATLAB_MEX_FILE
#   include "mex.h"
#else
#   include "simulator.h"
#endif

#define MY_NODE		0

/*
 * Функция пользователя.
 */
void node0 (cpu_t *c)
{
	int cluster_mode, start_node;
	unsigned long cluster_time, local_time, prev_time;
	unsigned short s, x;

	/* Проверка и печать номера и даты ревизии контроллера TTP. */
	node_check_revision (c);

	/* Сброс контроллера в исходное состояние. */
	cpu_write (c, TTC_GCR, TTC_GCR_GRST);
	cpu_write (c, TTC_GCR, 0);

	/* Тест шины. */
/*	node_test_bus (c);*/

	/* Установка регистров, режим 1. */
	node_setup (c, MY_NODE);
	node_set_mode (c, 1);

	/* Зададим значения переменным. */
	cpu_write (c, ADDR(x0), 0xaa12);
	cpu_write (c, ADDR(x1), 0xaa12);

	/* Активный старт. */
	cpu_write (c, TTC_NMR, TTC_MR_RXEN0 | TTC_MR_RXEN1 | TTC_MR_STRT | 1);
	cpu_write (c, TTC_GCR, TTC_GCR_GRUN);

	/*---------------------------
	 * 1) Ждем стартового пакета.
	 */
	for (;;) {
		/* Очистим статусы приёмников. */
		cpu_write (c, TTC_RSR(0), ~0);
		cpu_write (c, TTC_RSR(1), ~0);

		/* Ждём в течение двух циклов. */
		if (node_wait_start_packet (c, &cluster_mode, &start_node,
		    &cluster_time, &local_time) ||
		    node_wait_start_packet (c, &cluster_mode, &start_node,
		    &cluster_time, &local_time)) {
			/* Принят стартовый пакет. */
			goto passive_start;
		}

		/* Нет стартового пакета в течение двух циклов.
		 * Проверяем, были ли приняты хотя бы какие-нибудь пакеты.
		 * В таком случае продолжаем ожидание. */
		s = cpu_read (c, TTC_RSR(0));
		if (s & (TTC_RSR_RDN | TTC_RSR_REV | TTC_RSR_RLE |
		    TTC_RSR_CSE | TTC_RSR_RCME))
			continue;
		s = cpu_read (c, TTC_RSR(1));
		if (s & (TTC_RSR_RDN | TTC_RSR_REV | TTC_RSR_RLE |
		    TTC_RSR_CSE | TTC_RSR_RCME))
			continue;

		/* На шине полная тишина. */
		break;
	}

	/*---------------------------
	 * 2) Включаем передатчик, оставаясь в режиме старта.
	 * Теперь мы и передаём стартовые пакеты, и продолжаем приём.
	 */
	cpu_write (c, TTC_NMR, TTC_MR_RXEN0 | TTC_MR_RXEN1 |
		TTC_MR_TXEN0 | TTC_MR_TXEN1 | TTC_MR_STRT | 1);

	for (;;) {
		/* Очистим статусы приёмников. */
		cpu_write (c, TTC_RSR(0), ~0);
		cpu_write (c, TTC_RSR(1), ~0);

		/* Ждём в течение двух циклов. */
		if (node_wait_start_packet (c, &cluster_mode, &start_node,
		    &cluster_time, &local_time) ||
		    node_wait_start_packet (c, &cluster_mode, &start_node,
		    &cluster_time, &local_time)) {
			/* Принят стартовый пакет.
			 * Игнорируем стартовые пакеты с бОльшим номером узла. */
			if (start_node < MY_NODE) {
				/* Отключаем передатчик. */
				cpu_write (c, TTC_NMR, TTC_MR_RXEN0 | TTC_MR_RXEN1 |
					TTC_MR_STRT | 1);
				goto passive_start;
			}
		}

		/* Нет стартового пакета в течение двух циклов.
		 * Проверяем, были ли приняты верные пакеты.
		 * В таком случае прерываем ожидание. */
		s = cpu_read (c, TTC_RSR(0));
		if (s & TTC_RSR_RDN)
			break;
		s = cpu_read (c, TTC_RSR(1));
		if (s & TTC_RSR_RDN)
			break;

		/* TODO: если нет приёма в течение N циклов, переходим
		 * в режим останова и требуем вмешательства оператора. */
	}

	/*---------------------------
	 * 3) Снимаем стартовый бит.
	 */
	cpu_write (c, TTC_NMR, TTC_MR_RXEN0 | TTC_MR_RXEN1 |
		TTC_MR_TXEN0 | TTC_MR_TXEN1 | 1);

	printf ("--%s-- successfully started\n", c->name);

	/* Рабочий цикл узла.
	 * В требуемые моменты времени вызываем соответствующие задачи. */
	prev_time = ~0;

	/* Вычисляем новое значение X. */
	x = 1;
	cpu_write (c, ADDR(x0), x);
	cpu_write (c, ADDR(x1), x);
	for (;;) {
		local_time = cpu_read32 (c, TTC_CTR);
		if (node_time_reached (SLOT2_TIME, local_time, prev_time)) {
			/* Узел 0, слот 2: вычисление X. */
			x = x + 1;
			cpu_write (c, ADDR(x0), x);
			cpu_write (c, ADDR(x1), x);
		}
		prev_time = local_time;
	}

passive_start:
	/*TODO*/
	;
}
