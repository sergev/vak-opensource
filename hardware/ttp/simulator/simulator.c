/*
 * Эмулятор кластера TTP.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulator.h"
#include "simstruct.h"

#define EPSILON		1e-10

extern void node0 (cpu_t *c);
extern void node1 (cpu_t *c);
#if NNODES > 2
extern void node2 (cpu_t *c);
#endif
#if NNODES > 3
extern void node3 (cpu_t *c);
#endif

static void (*node_start [NNODES]) () = {
	node0,
	node1,
#if NNODES > 2
	node2,
#endif
#if NNODES > 3
	node3,
#endif
};

#define CONTEXT_SAVE(c,ret) 	__asm__ volatile (	\
	"movl %0, %%eax \n"				\
	"movl %%ebp, (%%eax) \n"			\
	"movl %%ebx, 4(%%eax) \n"			\
	"movl %%edi, 8(%%eax) \n"			\
	"movl %%esi, 12(%%eax) \n"			\
	"movl %%esp, 16(%%eax) \n"			\
	"movl $"ret", 20(%%eax)"			\
	: : "r" (c))

#define CONTEXT_RESTORE(c) 	__asm__ volatile (	\
	"movl %0, %%eax \n"				\
	"movl (%%eax), %%ebp \n"			\
	"movl 4(%%eax), %%ebx \n"			\
	"movl 8(%%eax), %%edi \n"			\
	"movl 12(%%eax), %%esi \n"			\
	"movl 16(%%eax), %%esp \n"			\
	"movl 20(%%eax), %%edx \n"			\
	"jmp *%%edx "					\
	: : "r" (c))

/*
 * Завершение цикла процессора.
 * Передача управления сопрограмме Simulink.
 * После возврата - начало нового цикла процессора.
 */
void cpu_tick (cpu_t *c)
{
	/* Процессор выполнил один шаг и установил значения выходных сигналов. */
	/*printf ("--%s-- datain=%x, ack=%d, dataout=%x, addr=%x, rd=%d, wrh=%d, wrl=%d\n",
		c->name, c->datain, c->ack,
		c->dataout, c->addr, c->rd, c->wrh, c->wrl);*/

	/* Останавливаемся и ждём следующего такта.  */
	CONTEXT_SAVE (&c->entry_ebp, "1f");
	CONTEXT_RESTORE (&c->return_ebp);
	__asm__ volatile ("1:");
}

/*
 * Цикл шины: запись 16-битного слова в память.
 */
void cpu_write (cpu_t *c, unsigned int addr, unsigned int val)
{
	c->addr = addr;
	c->dataout = val;
	cpu_tick (c);

	c->wrh = c->wrl = 1;
	do {
		cpu_tick (c);
	} while (! c->ack);

	c->wrh = c->wrl = 0;
}

/*
 * Запись 32-битного слова в память (два цикла).
 */
void cpu_write32 (cpu_t *c, unsigned int addr, unsigned long val)
{
	cpu_write (c, addr, (unsigned short) val);
	cpu_write (c, addr + 2, (unsigned short) (val >> 16));
}

/*
 * Цикл шины: чтение 16-битного слова из памяти.
 */
unsigned int cpu_read (cpu_t *c, unsigned int addr)
{
	unsigned int val;

	c->addr = addr;
	cpu_tick (c);

	c->rd = 1;
	do {
		cpu_tick (c);
	} while (! c->ack);

	val = c->datain;
	c->rd = 0;
	return val;
}

/*
 * Чтение 32-битного слова из памяти.
 */
unsigned long cpu_read32 (cpu_t *c, unsigned int addr)
{
	unsigned long val;

	val = cpu_read (c, addr);
	val |= cpu_read (c, addr + 2) << 16;
	return val;
}

/*
 * Сброс процесора, эмулятор начинает работу с начала.
 */
static void cpu_reset (cpu_t *c, void (*func) (cpu_t *c))
{
	if (c->name)
		printf ("--%s-- reset\n", c->name);
	if (func)
		c->reset_func = func;
	c->entry_ebx = (unsigned) c->reset_func;
	c->entry_edi = (unsigned) c;
	c->entry_esp = (unsigned) c + STACK_BYTES - sizeof (unsigned);
	c->entry_eip = c->reset_eip;

	c->dataout = 0;
	c->addr = 0;
	c->rd = 0;
	c->wrh = 0;
	c->wrl = 0;
}

/*
 * Выделение памяти для эмулятора, старт потока.
 */
static cpu_t *cpu_alloc (void (*func) (cpu_t *c))
{
	cpu_t *c;

	c = (cpu_t*) calloc (1, STACK_BYTES);
	if (! c) {
		fprintf (stderr, "%s: out of memory\n", c->name);
		exit (1);
	}
	__asm__ volatile (
	"	jmp 2f \n"
	/* Subtask */
	"1:	movl %edi, (%esp) \n"
	"	call *%ebx \n"
	"	jmp 1b \n"
	"2:");

	CONTEXT_SAVE (&c->entry_ebp, "1b");
	c->reset_eip = c->entry_eip;
	cpu_reset (c, func);
	return c;
}

/*
 * Выполнение одного шага процессора.
 */
static void cpu_step (cpu_t *c)
{
	/*printf ("--%s-- step\n", c->name);*/

	/* Переключение на сопроцесс эмулятора. */
	CONTEXT_SAVE (&c->return_ebp, "1f");
	CONTEXT_RESTORE (&c->entry_ebp);
	__asm__ volatile ("1:");
}

/*
 * Инициализация симулятора, выделение памяти для всех модулей,
 * старт подчинённых потоков.
 */
void simulator_init (simulator_t *sim)
{
	int i;
	static const char *ctlr_names [] = { "ttp0", "ttp1", "ttp2", "ttp3" };
	static const char *node_names [] = { "node0", "node1", "node2", "node3" };

	for (i=0; i<NNODES; ++i) {
		sim->ctlr[i] = ctlr_alloc ();
		sim->ctlr[i]->name = ctlr_names [i];

		sim->node[i] = cpu_alloc (node_start[i]);
		sim->node[i]->name = node_names [i];
	}
	sim->bus0 = bus_alloc ();
	sim->bus0->name = "bus0";
	sim->bus1 = bus_alloc ();
	sim->bus1->name = "bus1";
}

/*
 * Приведение симулятора в исходное состояние.
 */
void simulator_reset (simulator_t *sim)
{
	int i;

	for (i=0; i<NNODES; ++i) {
		cpu_reset (sim->node[i], 0);
		ctlr_reset (sim->ctlr[i]);
		sim->node_time_last [i] = 0;
		sim->ctlr_time_last [i] = 0;
		ui_draw_cpu (sim, i, 0);
		ui_draw_ctlr (sim, i, 0);
	}
	bus_reset (sim->bus0);
	bus_reset (sim->bus1);

	/* Обнуляем текущее время и моменты последнего такта. */
	sim->nstep = 0;
	sim->time_usec = 0;
}

/*
 * Выполнение одного шага симулятора.
 */
void simulator_step (simulator_t *sim)
{
	int i, need_bus_step = 0;
	double time_next, tn;

	/* Вычисляем шаг по времени.
	 * Берём минимальный шаг по всем модулям. */
	time_next = sim->time_usec + 1000000;
	for (i=0; i<NNODES; ++i) {
		tn = sim->node_time_last [i] + sim->node_time_step [i];
/*printf ("%s tn=%.8g\n", sim->node[i]->name, tn);*/
		if (time_next > tn)
			time_next = tn;
		tn = sim->ctlr_time_last [i] + sim->ctlr_time_step [i];
/*printf ("%s tn=%.8g\n", sim->ctlr[i]->name, tn);*/
		if (time_next > tn)
			time_next = tn;
	}

	/* Проходим по всем модулям и делаем шаг для тех,
	 * которые попадают по времени. */
	for (i=0; i<NNODES; ++i) {
		if (sim->ctlr_time_last [i] + sim->ctlr_time_step [i] - time_next <
		    EPSILON) {
			/* TTP-контроллер узла #i, коммуникационная часть */
			sim->ctlr[i]->rx0    = sim->bus0->tx;
			sim->ctlr[i]->rx1    = sim->bus1->tx;
			ctlr_step_rxtx (sim->ctlr[i]);

			sim->ctlr_time_last [i] = time_next;
			need_bus_step = 1;

			/* Подкрашиваем активный блок. */
			ui_draw_ctlr (sim, i, 1);
		} else
			ui_draw_ctlr (sim, i, 0);

		if (sim->node_time_last [i] + sim->node_time_step [i] - time_next <
		    EPSILON) {
			/* Процессор узла #i */
			sim->node[i]->datain = sim->ctlr[i]->dataout;
			sim->node[i]->ack    = sim->ctlr[i]->ack;
			if (sim->rst[i]) {
				/* Сброс процессора. */
				cpu_reset (sim->node[i], 0);
			} else
				cpu_step (sim->node[i]);

			/* TTP-контроллер узла #i, процессорная часть */
			sim->ctlr[i]->datain = sim->node[i]->dataout;
			sim->ctlr[i]->addr   = sim->node[i]->addr;
			sim->ctlr[i]->rd     = sim->node[i]->rd;
			sim->ctlr[i]->wrh    = sim->node[i]->wrh;
			sim->ctlr[i]->wrl    = sim->node[i]->wrl;
			ctlr_step_cpu (sim->ctlr[i]);

			sim->node_time_last [i] = time_next;

			/* Подкрашиваем активный блок. */
			ui_draw_cpu (sim, i, 1);
		} else
			ui_draw_cpu (sim, i, 0);
	}

	if (need_bus_step) {
		/* Шины */
		sim->bus0->rn0 = sim->ctlr[0]->tx;
		sim->bus1->rn0 = sim->ctlr[0]->tx;
		sim->bus0->rn1 = sim->ctlr[1]->tx;
		sim->bus1->rn1 = sim->ctlr[1]->tx;
#if NNODES > 2
		sim->bus0->rn2 = sim->ctlr[2]->tx;
		sim->bus1->rn2 = sim->ctlr[2]->tx;
#else
		sim->bus0->rn2 = 1;
		sim->bus1->rn2 = 1;
#endif
#if NNODES > 3
		sim->bus0->rn3 = sim->ctlr[3]->tx;
		sim->bus1->rn3 = sim->ctlr[3]->tx;
#else
		sim->bus0->rn3 = 1;
		sim->bus1->rn3 = 1;
#endif
		bus_step (sim->bus0);
		bus_step (sim->bus1);
	}
	++sim->nstep;
	sim->time_usec = time_next;
}

void simulator_set_cpu_frequency (simulator_t *sim, int node_num, int val)
{
	sim->cpu_mhz [node_num] = val;

	/* Устанавливаем длительность такта процессора в микросекундах. */
	sim->node_time_step [node_num] = 1.0 / sim->cpu_mhz [node_num];
	printf ("CPU %d frequency set to %d MHz, time step %g usec.\n",
		node_num, sim->cpu_mhz [node_num], sim->node_time_step [node_num]);
}

void simulator_set_bus_rate (simulator_t *sim, int val)
{
	int i;

	sim->bus_mbps = val;
	printf ("Bus data rate changed to %d Mbps.\n", sim->bus_mbps);

	/* Устанавливаем длительность такта контроллеров в микросекундах. */
	for (i=0; i<NNODES; ++i) {
		sim->ctlr_time_step [i] = (1.0 + sim->ctlr_ppm [i] / 1000000.0) /
			sim->bus_mbps;
		printf ("TTP controller %d time step %g usec.\n",
			i, sim->ctlr_time_step [i]);
	}
}

void simulator_set_ctlr_frequency (simulator_t *sim, int node_num, int val)
{
	sim->ctlr_ppm [node_num] = val;

	/* Устанавливаем длительность такта контроллеров в микросекундах. */
	sim->ctlr_time_step [node_num] = (1.0 + sim->ctlr_ppm [node_num] / 1000000.0) /
		sim->bus_mbps;
	printf ("TTP controller %d frequency precision set to %+d ppm, time step %g usec.\n",
		node_num, sim->ctlr_ppm [node_num], sim->ctlr_time_step [node_num]);
}
