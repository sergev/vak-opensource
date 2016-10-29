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

#define MY_NODE		1

/*
 * Функция пользователя.
 */
void node1 (cpu_t *c)
{
	int cluster_mode, start_node;
	unsigned long cluster_time, local_time, prev_time;
	unsigned short x, y;

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
	cpu_write (c, ADDR(y0), 0);
	cpu_write (c, ADDR(y1), 0);

	/* Пассивный старт. */
	cpu_write (c, TTC_NMR, TTC_MR_RXEN0 | TTC_MR_RXEN1 | TTC_MR_STRT | 1);
	cpu_write (c, TTC_GCR, TTC_GCR_GRUN);

	/*---------------------------
	 * 1) Ждем стартового пакета.
	 */
	for (;;) {
		if (node_wait_start_packet (c, &cluster_mode, &start_node,
		    &cluster_time, &local_time))
			break;
	}
	cpu_write (c, TTC_GSR, TTC_GSR_CCL);

	/*---------------------------
	 * 2) Синхронизируемся.
	 */
	printf ("--%s--0 cluster mode=%d, start node=%d, cluster time=%ld, local time=%ld\n",
		c->name, cluster_mode, start_node, cluster_time, local_time);

	/* Устанавливаем режим кластера cluster_mode. */
	node_set_mode (c, cluster_mode);

	/* Снимаем стартовый бит. */
	cpu_write (c, TTC_NMR, TTC_MR_RXEN0 | TTC_MR_RXEN1 | cluster_mode);

	/* Корректируем время на основе cluster_time и local_time. */
	node_set_cycle_duration (c, 1, local_time - cluster_time);

	/* Ждем конца цикла, чтобы перейти на новый режим и время. */
	node_wait_gsr (c, TTC_GSR_CCL);

	/*---------------------------
	 * 3) Включаем передатчик.
	 * Перед этим ждем еще один полный цикл кластера, чтобы подсинхронизироваться.
	 */
	cpu_write (c, TTC_NMR, TTC_MR_RXEN0 | TTC_MR_RXEN1 |
		TTC_MR_TXEN0 | TTC_MR_TXEN0 | cluster_mode);
	cpu_write (c, TTC_GSR, TTC_GSR_CCL);
	node_wait_gsr (c, TTC_GSR_CCL);

	printf ("--%s-- successfully started\n", c->name);

	/* Рабочий цикл узла.
	 * В требуемые моменты времени вызываем соответствующие задачи. */
	prev_time = ~0;
	for (;;) {
		local_time = cpu_read32 (c, TTC_CTR);
		if (node_time_reached (SLOT1_TIME, local_time, prev_time)) {
			/* Узел 1, слот 1: вычисление Y в зависимости от X. */
			if (node_get_ushort (c, &x, ADDR(x0), ADDR(x0_status),
			    ADDR(x1), ADDR(x1_status))) {
				/* Вычисляем новое значение Y. */
				y = x * x;
				cpu_write (c, ADDR(y0), y);
				cpu_write (c, ADDR(y1), y);
			} else {
				/* Не удалось получить значение X.
				 * Требуется какое-то аварийное действие. */
			}
		}
		prev_time = local_time;
	}
}
