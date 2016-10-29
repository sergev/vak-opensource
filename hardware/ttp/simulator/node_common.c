/*
 * Данные и функции, общие для всех узлов.
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

/* Уникальный идентификатор расписания. */
unsigned long medl_sid = 0x12345678;

/* Расписание кластера.
 * Четыре узла, каждый в свою очередь передаёт двухбайтовое значение.
 * Синхронизация в конце цикла.
 */
const medl_t medl_data [] = {
/*  time         node  bytes     addr0         addr1                addr2   flags */
/* режим 0 - приём при старте */
{ 0,               0,     6,  ADDR(start0_status), ADDR(start1_status), 0,  TTC_MEDL_START },

/* режим 1 - нормальная работа или передатчик при активном старте */
{ SLOT0_DURATION,  0,     2,  ADDR(x0_status), ADDR(x1_status),         0,  0 },
{ SLOT1_DURATION,  0,     6,  ADDR(s0_status), ADDR(s1_status),         0,  TTC_MEDL_START },
{ SLOT2_DURATION,  1,     2,  ADDR(y0_status), ADDR(y1_status),         0,  0 },
};

/*
 * Индексы таблицы расписания для разных режимов.
 */
const int medl_index [] = {
	0,	/* режим 0 */
	1,	/* режим 1 */
};

/*
 * Чтение и проверка номера ревизии контроллера.
 */
void node_check_revision (cpu_t *c)
{
	unsigned int frr, fdr;

	frr = cpu_read (c, TTC_FRR);
	if (frr == 0 || frr == 0xffff) {
		printf ("--%s-- incorrect FRR value %04x\n", c->name, frr);
		printf ("--%s-- halted\n", c->name);
		for (;;)
			cpu_read (c, TTC_FRR);
	}
	fdr = cpu_read (c, TTC_FDR);
	if (fdr == 0 || fdr == 0xffff) {
		printf ("--%s-- incorrect FDR value %04x\n", c->name, frr);
		printf ("--%s-- halted\n", c->name);
		for (;;)
			cpu_read (c, TTC_FDR);
	}
	printf ("%s firmware: revision %c%x %x%x/%x%x/20%x%x\n", c->name,
			(frr >> 12) + 'A', (frr >> 8) & 15,
			(fdr >> 4) & 15, (fdr & 15),
			(fdr >> 12) & 15, (fdr >> 8) & 15,
			(frr >> 4) & 15, (frr & 15));
}

/*
 * Тест шины между процессором и контроллером.
 * Используем регистр SID для записи и чтения.
 */
void node_test_bus (cpu_t *c)
{
	int i, val;

	for (i=0; i<16; ++i) {
		cpu_write (c, TTC_SID, 1 << i);
		val = cpu_read (c, TTC_SID);
		if (val != 1 << i) {
			printf ("--%s-- data bus error: written %04x read %04x\n",
				c->name, 1 << i, val);
			printf ("--%s-- halted\n", c->name);
			for (;;) {
				cpu_write (c, TTC_SID, 1 << i);
				cpu_read (c, TTC_SID);
			}
		}
	}
	printf ("%s test: succeeded\n", c->name);
}

/*
 * Установка номера узла и таблицы расписания MEDL.
 */
void node_setup (cpu_t *c, int node_num)
{
	const unsigned short *p;
	int addr;

	/* Номер узла в кластере. */
	cpu_write (c, TTC_NID, node_num);

	/* Уникальный идентификатор расписания. */
	cpu_write32 (c, TTC_SID, medl_sid);

	/* Длительность цикла кластера. */
	node_set_cycle_duration (c, 0, CYCLE_DURATION);

	/* Запись таблицы расписания. */
	p = (const unsigned short*) medl_data;
	for (addr=0; addr<sizeof(medl_data); addr+=2, p++) {
		cpu_write (c, addr, *p);
	}
	cpu_write (c, TTC_SSRP, medl_index [0] * 16);
}

/*
 * Установка режима кластера.
 */
void node_set_mode (cpu_t *c, int cluster_mode)
{
	cpu_write (c, TTC_SSR, medl_index [cluster_mode] * 16);
}

/*
 * Установка длительности цикла кластера.
 * Флаг one_cycle_only - установка только на один цикл кластера, регистр MTR.
 * Иначе - оба регистра MTR и NMTR.
 * Отрицательная длительность задаёт длительность относительно CYCLE_DURATION.
 */
void node_set_cycle_duration (cpu_t *c, int one_cycle_only, signed long duration)
{
	if (duration == 0)
		return;
	if (duration < 0)
		duration += CYCLE_DURATION;

	cpu_write32 (c, TTC_MTR, duration);
	if (! one_cycle_only)
		cpu_write32 (c, TTC_NMTR, duration);
}

/*
 * Ожидание стартового пакета в течение одного цикла.
 * Возврат 1, если пакет получен.
 */
int node_wait_start_packet (cpu_t *c, int *cluster_mode, int *start_node,
	unsigned long *cluster_time, unsigned long *local_time)
{
	cpu_write (c, TTC_GSR, TTC_GSR_CCL);
	for (;;) {
		int s;

		s = cpu_read (c, ADDR(start0_status));
		if (s) {
			printf ("--%s--0 receive status %02x\n", c->name, s);
			if (s == (TTC_RSR_RDN | TTC_RSR_STRT)) {
				*cluster_mode = cpu_read (c, ADDR(start0_cmode));
				*start_node = cpu_read (c, ADDR(start0_node));
				*cluster_time = cpu_read32 (c, ADDR(start0_cluster_time));
				*local_time = cpu_read32 (c, ADDR(start0_cluster_time));
				return 1;
			}
			cpu_write (c, ADDR(start0_status), 0);
		}
		s = cpu_read (c, ADDR(start1_status));
		if (s) {
			printf ("--%s--1 receive status %02x\n", c->name, s);
			if (s == (TTC_RSR_RDN | TTC_RSR_STRT)) {
				*cluster_mode = cpu_read (c, ADDR(start1_cmode));
				*start_node = cpu_read (c, ADDR(start1_node));
				*cluster_time = cpu_read32 (c, ADDR(start1_cluster_time));
				*local_time = cpu_read32 (c, ADDR(start1_cluster_time));
				return 1;
			}
			cpu_write (c, ADDR(start1_status), 0);
		}
		s = cpu_read (c, TTC_GSR);
		if (s & TTC_GSR_CCL) {
			/* Закончился цикл кластера. */
			return 0;
		}
	}
}

/*
 * Ожидание битов в регистре GSR.
 * Возвращает текущее значение регистра GSR.
 */
int node_wait_gsr (cpu_t *c, int bit)
{
	int s;

	for (;;) {
		s = cpu_read (c, TTC_GSR);
		if (s & bit)
			return s;
	}
}

/*
 * Проверка наступления ожидаемого момента времени.
 */
int node_time_reached (unsigned long slot_time,
	unsigned long current_time, unsigned long prev_time)
{
	/* Текущее время должно быть больше ожидаемого. */
	if (current_time < slot_time)
		return 0;

	/* Предыдущее время должно быть меньшее ожидаемого,
	 * с учётом перехода через границу цикла. */
	if (prev_time < current_time && prev_time >= slot_time)
		return 0;

	return 1;
}

/*
 * Извлечение 16-битного значения из контроллера TTP.
 */
int node_get_ushort (cpu_t *c, unsigned short *result,
	unsigned short addr0, unsigned short addr_status0,
	unsigned short addr1, unsigned short addr_status1)
{
	unsigned short s;

	/* Анализируем статусы обеих копий и выбираем правильное значение. */
	s = cpu_read (c, addr_status0);
	if (s & TTC_RSR_RDN) {
		*result = cpu_read (c, addr0);
		return 1;
	}
	s = cpu_read (c, addr_status1);
	if (s & TTC_RSR_RDN) {
		*result = cpu_read (c, addr1);
		return 1;
	}
	return 0;
}
