/*
 * Эмулятор кластера TTP.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#include "ctlr.h"
#include "bus.h"
#include "cpu.h"

#define NNODES			2	/* Количество узлов в кластере */
#define STACK_BYTES		64000	/* Размер стека для сопроцесса */

typedef struct _simulator_t {
	/* Модули, образующие кластер TTP */
	controller_t *ctlr [NNODES];		/* контроллеры TTP */
	cpu_t *node [NNODES];			/* процессоры */
	bus_t *bus0, *bus1;			/* шины */

	/* Численные параметры модели. */
	int rst [NNODES];			/* сброс процессоров */
	int bus_mbps;				/* скорость шин */
	int cpu_mhz [NNODES];			/* частота процессоров */
	int ctlr_ppm [NNODES];			/* точность частоты контроллеров */

	/* Время моделирования, микросекунды. */
	int nstep;				/* номер шага */
	double time_usec;			/* текущее время */
	double node_time_step [NNODES];		/* длительность такта процессоров */
	double ctlr_time_step [NNODES];		/* длительность такта контроллеров */
	double node_time_last [NNODES];		/* момент последнего такта процессоров */
	double ctlr_time_last [NNODES];		/* момент последнего такта контроллеров */

#ifdef GTK_WINDOW
	/* История, nstep элементов. */
	GArray *history;

	/* Части пользовательского интерфейса. */
	GladeXML *ui;
	GtkStatusbar *statusbar;
	GtkImage *schematics;
	GtkDrawingArea *chart;
	GtkComboBox *bus_combobox;
	GtkComboBox *cpu_combobox [NNODES];
	GtkComboBox *ctlr_combobox [NNODES];
	GtkCheckButton *cpu_reset_checkbutton [NNODES];
	GdkGC *gc_yellow;
	GdkGC *gc_dotted;
	int chart_needs_redraw;
	int run_continuously;

	/* Координаты графических элементов на схеме кластера. */
	int dx, dy;
	int node_left [NNODES], node_center [NNODES];
	int cpu_top, ctlr_top, bus_top;
	int cpu_was_active [NNODES], ctlr_was_active [NNODES];
#endif /* GTK_WINDOW */

} simulator_t;

void simulator_init (simulator_t *sim);
void simulator_reset (simulator_t *sim);
void simulator_step (simulator_t *sim);
void simulator_set_cpu_frequency (simulator_t *sim, int node_num, int mhz);
void simulator_set_bus_rate (simulator_t *sim, int mhz);
void simulator_set_ctlr_frequency (simulator_t *sim, int node_num, int ppm);

void ui_draw_cpu (simulator_t *sim, int i, int active);
void ui_draw_ctlr (simulator_t *sim, int i, int active);
