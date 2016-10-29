/*
 * Оконный интерфейс GTK+ для эмулятора кластера TTP.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include "simulator.h"
#include "simstruct.h"

#define LABEL_WIDTH	40

/*
 * Слепок состояния всех сигналов модели.
 */
typedef struct _history_t {
	int cpu_rst [NNODES];			/* сброс процессоров */
	unsigned int cpu_dataout [NNODES];
	unsigned int cpu_addr [NNODES];
	int cpu_rd [NNODES];
	int cpu_wrh [NNODES];
	int cpu_wrl [NNODES];

	unsigned int ctlr_dataout [NNODES];
	int ctlr_ack [NNODES];
	int ctlr_tx [NNODES];

	int bus0_tx, bus1_tx;

	int nstep;				/* номер шага */
	int slot_start;				/* начало слота */
	int cycle_start;			/* начало цикла */
	double time_usec;			/* время в миллисекундах */
} history_t;

static GtkTextView *textview_log;
static void status_print (simulator_t *sim, const char *fmt, ...)
	__attribute__((__format__ (__printf__, 2, 3)));

/*
 * Выдача текста в журнал событий.
 * Используется вместо printf().
 */
void ui_print (const char *fmt, ...)
{
	char buf [512];
	va_list ap;
	GtkTextBuffer *buffer;
	GtkTextMark *end_mark;
	GtkTextIter end_iter;

	va_start (ap, fmt);
	vsnprintf (buf, sizeof(buf), fmt, ap);
	va_end (ap);

	buffer = gtk_text_view_get_buffer (textview_log);
	end_mark = gtk_text_buffer_get_mark (buffer, "end");
	gtk_text_buffer_get_iter_at_mark (buffer, &end_iter, end_mark);
	gtk_text_buffer_insert (buffer, &end_iter, buf, -1);

	gtk_text_view_scroll_mark_onscreen (textview_log, end_mark);
}

static void status_print (simulator_t *sim, const char *fmt, ...)
{
	char buf [512];
	va_list ap;

	va_start (ap, fmt);
	vsnprintf (buf, sizeof(buf), fmt, ap);
	va_end (ap);

	gtk_statusbar_pop (sim->statusbar, 0);
	gtk_statusbar_push (sim->statusbar, 0, buf);
}

/*
 * Подкрашиваем активный блок.
 */
void ui_draw_ctlr (simulator_t *sim, int i, int active)
{
	GtkWidget *parent;
	GdkPixmap *pixmap;
	GdkGC *gc;

	if (sim->run_continuously && ! (sim->ctlr_was_active[i] && ! active))
		return;
	parent = gtk_widget_get_parent (GTK_WIDGET(sim->schematics));
	if (active) {
		/* Подкрашиваем фон. */
		sim->ctlr_was_active[i] = 1;
		gc = sim->gc_yellow;

	} else if (sim->ctlr_was_active[i]) {
		/* Стираем подкрашенное. */
		sim->ctlr_was_active[i] = 0;
		gc = parent->style->white_gc;
	} else
		return;

	gtk_image_get_pixmap (sim->schematics, &pixmap, 0);
	gdk_draw_rectangle (pixmap, gc, TRUE,
		sim->node_left[i], sim->ctlr_top, sim->dx, sim->dy);
	gdk_draw_rectangle (pixmap, parent->style->black_gc, FALSE,
		sim->node_left[i], sim->ctlr_top, sim->dx, sim->dy);
/*	gtk_widget_queue_draw_area (GTK_WIDGET(sim->schematics), sim->node_left[i], sim->ctlr_top, sim->dx, sim->dy);*/
	gtk_image_set_from_pixmap (sim->schematics, pixmap, 0);
}

void ui_draw_cpu (simulator_t *sim, int i, int active)
{
	GtkWidget *parent;
	GdkPixmap *pixmap;
	GdkGC *gc;

	if (sim->run_continuously && ! (sim->cpu_was_active[i] && ! active))
		return;
	parent = gtk_widget_get_parent (GTK_WIDGET(sim->schematics));
	if (active) {
		/* Подкрашиваем фон. */
		sim->cpu_was_active[i] = 1;
		gc = sim->gc_yellow;

	} else if (sim->cpu_was_active[i]) {
		/* Стираем подкрашенное. */
		sim->cpu_was_active[i] = 0;
		gc = parent->style->white_gc;
	} else
		return;

	gtk_image_get_pixmap (sim->schematics, &pixmap, 0);
	gdk_draw_rectangle (pixmap, gc, TRUE,
		sim->node_left[i], sim->cpu_top, sim->dx, sim->dy);
	gdk_draw_rectangle (pixmap, parent->style->black_gc, FALSE,
		sim->node_left[i], sim->cpu_top, sim->dx, sim->dy);
/*	gtk_widget_queue_draw_area (GTK_WIDGET(sim->schematics), sim->node_left[i], sim->cpu_top, sim->dx, sim->dy);*/
	gtk_image_set_from_pixmap (sim->schematics, pixmap, 0);
}

static void clear_history (simulator_t *sim)
{
	if (! sim->history)
		sim->history = g_array_new (FALSE, TRUE, sizeof (history_t));
	g_array_set_size (sim->history, 0);
}

static void append_history (simulator_t *sim)
{
	history_t val;
	int i;

	for (i=0; i<NNODES; ++i) {
		val.cpu_rst [i] = sim->rst [i];

		val.cpu_dataout [i] = sim->node[i]->dataout;
		val.cpu_addr [i] = sim->node[i]->addr;
		val.cpu_rd [i] = sim->node[i]->rd;
		val.cpu_wrh [i] = sim->node[i]->wrh;
		val.cpu_wrl [i] = sim->node[i]->wrl;

		val.ctlr_dataout [i] = sim->ctlr[i]->dataout;
		val.ctlr_ack [i] = sim->ctlr[i]->ack;
		val.ctlr_tx [i] = sim->ctlr[i]->tx;

	}
	val.bus0_tx = sim->bus0->tx;
	val.bus1_tx = sim->bus1->tx;

	val.nstep = sim->nstep;
	val.time_usec = sim->time_usec;

	val.cycle_start = (sim->ctlr[0]->gcr & TTC_GCR_GRUN) &&
		(sim->ctlr[0]->ctr == 0);
	val.slot_start = (sim->ctlr[0]->gcr & TTC_GCR_GRUN) &&
		(sim->ctlr[0]->ctr == sim->ctlr[0]->slot_time);
	g_array_append_val (sim->history, val);
}

/*
 * Пересекаются ли два прямоугольника.
 */
static int intersect (int x0, int y0, int w0, int h0,
	int x1, int y1, int w1, int h1)
{
	GdkRectangle rect0, rect1;

	rect0.x = x0;
	rect0.y = y0;
	rect0.width = w0;
	rect0.height = h0;
	rect1.x = x1;
	rect1.y = y1;
	rect1.width = w1;
	rect1.height = h1;
	return gdk_rectangle_intersect (&rect0, &rect1, 0);
}

/*
 * Стирание диаграммы.
 */
static void chart_clear (simulator_t *sim)
{
        GtkWidget *widget = GTK_WIDGET (sim->chart);
	GdkRectangle rect;

	if (! GTK_WIDGET_MAPPED (sim->chart))
		return;
	rect.x = 0;
	rect.y = 0;
	rect.width = widget->allocation.width - LABEL_WIDTH + 1;
	rect.height = widget->allocation.height;
	gdk_window_invalidate_rect (widget->window, &rect, 0);
}

/*
 * Продвижение диаграммы.
 */
static void chart_step (simulator_t *sim)
{
        GtkWidget *widget = GTK_WIDGET (sim->chart);
	GdkRegion *region;
	GdkPoint points [4];

	if (! GTK_WIDGET_MAPPED (sim->chart))
		return;
	points[0].x = 0;
	points[0].y = 0;

	points[1].x = widget->allocation.width - LABEL_WIDTH + 1;
	points[1].y = 0;

	points[2].x = widget->allocation.width - LABEL_WIDTH + 1;
	points[2].y = widget->allocation.height;

	points[3].x = 0;
	points[3].y = widget->allocation.height;

	region = gdk_region_polygon (points, 4, GDK_EVEN_ODD_RULE);
	gdk_window_move_region (widget->window, region, -2, 0);
	gdk_region_destroy (region);
}

/*
 * Отрисовка границы слота.
 */
static void chart_draw_slot_border (simulator_t *sim, history_t *val, int x)
{
        GtkWidget *widget = GTK_WIDGET (sim->chart);

	gdk_draw_line (widget->window, val->cycle_start ?
		widget->style->bg_gc [GTK_STATE_NORMAL] : sim->gc_dotted,
		x, 0, x, widget->allocation.height);
}

/*
 * Отрисовка одного сигнала.
 */
static void chart_draw_signal (simulator_t *sim, int current, int old,
	int x, int y, int dy)
{
        GtkWidget *widget = GTK_WIDGET (sim->chart);

	if (old != current) {
		/* Вертикальная линия. */
		gdk_draw_line (widget->window, sim->gc_yellow, x+2, y-dy, x+2, y);
	}
	/* Горизонтальная линия. */
	if (current) {
		gdk_draw_line (widget->window, sim->gc_yellow, x, y-dy, x+2, y-dy);
	} else {
		gdk_draw_line (widget->window, sim->gc_yellow, x, y, x+2, y);
	}
}

/*
 * Отрисовка диаграммы.
 */
static void chart_draw (simulator_t *sim, int area_x, int area_y,
	int area_width, int area_height)
{
	GtkWidget *widget = GTK_WIDGET (sim->chart);
	GdkFont *font;
	history_t val, last;
	int n, x, dy;

	/* Фон. */
	gdk_draw_rectangle (widget->window, widget->style->black_gc, TRUE,
		area_x, area_y, area_width, area_height);

	dy = widget->allocation.height / (NNODES * 6 + 1);
	for (n=0; n<NNODES*3; ++n) {
		gdk_draw_line (widget->window, sim->gc_dotted,
			0, 2*(n+1)*dy, widget->allocation.width - LABEL_WIDTH + 1, 2*(n+1)*dy);
	}

	/* Метки сигналов. */
	if (intersect (area_x, area_y, area_width, area_height,
	    widget->allocation.width - LABEL_WIDTH, 0,
	    LABEL_WIDTH, widget->allocation.height)) {
		font = gdk_font_from_description (widget->style->font_desc);
		gdk_draw_string (widget->window, font, sim->gc_dotted,
			widget->allocation.width - LABEL_WIDTH + 5, 2*1*dy, "RD 0");
		gdk_draw_string (widget->window, font, sim->gc_dotted,
			widget->allocation.width - LABEL_WIDTH + 5, 2*2*dy, "WRL 0");
		gdk_draw_string (widget->window, font, sim->gc_dotted,
			widget->allocation.width - LABEL_WIDTH + 5, 2*3*dy, "RD 1");
		gdk_draw_string (widget->window, font, sim->gc_dotted,
			widget->allocation.width - LABEL_WIDTH + 5, 2*4*dy, "WRL 1");
		gdk_draw_string (widget->window, font, sim->gc_dotted,
			widget->allocation.width - LABEL_WIDTH + 5, 2*5*dy, "TX 0");
		gdk_draw_string (widget->window, font, sim->gc_dotted,
			widget->allocation.width - LABEL_WIDTH + 5, 2*6*dy, "TX 1");
	}
	if (sim->nstep <= 0) {
		/* Нечего рисовать. */
		return;
	}
	/* Последний шаг. */
	last = g_array_index (sim->history, history_t, sim->nstep-1);
	x = widget->allocation.width - LABEL_WIDTH;
	if (last.slot_start || last.cycle_start)
		chart_draw_slot_border (sim, &last, x);
	x -= 2;
	for (n=sim->nstep-2; n>=0 && x>=0; --n) {
		/* Предыдущий шаг. */
		if (! intersect (area_x, area_y, area_width, area_height,
		    x, 0, 3, widget->allocation.height))
			continue;
		val = g_array_index (sim->history, history_t, n);
		if (val.slot_start || val.cycle_start)
			chart_draw_slot_border (sim, &val, x);
		chart_draw_signal (sim, val.cpu_rd [0],  last.cpu_rd [0],  x, 2*1*dy, dy);
		chart_draw_signal (sim, val.cpu_wrl [0], last.cpu_wrl [0], x, 2*2*dy, dy);
		chart_draw_signal (sim, val.cpu_rd [1],  last.cpu_rd [1],  x, 2*3*dy, dy);
		chart_draw_signal (sim, val.cpu_wrl [1], last.cpu_wrl [1], x, 2*4*dy, dy);
		chart_draw_signal (sim, val.ctlr_tx [0], last.ctlr_tx [0], x, 2*5*dy, dy);
		chart_draw_signal (sim, val.ctlr_tx [1], last.ctlr_tx [1], x, 2*6*dy, dy);
		last = val;
		x -= 2;
	}
	if (x >= 0 && intersect (area_x, area_y, area_width, area_height,
	    x, 0, 3, widget->allocation.height)) {
		/* Начальное значение - нулевое. */
		chart_draw_signal (sim, 0, last.cpu_rd [0],  x, 2*1*dy, dy);
		chart_draw_signal (sim, 0, last.cpu_wrl [0], x, 2*2*dy, dy);
		chart_draw_signal (sim, 0, last.cpu_rd [1],  x, 2*3*dy, dy);
		chart_draw_signal (sim, 0, last.cpu_wrl [1], x, 2*4*dy, dy);
		chart_draw_signal (sim, 1, last.ctlr_tx [0], x, 2*5*dy, dy);
		chart_draw_signal (sim, 1, last.ctlr_tx [1], x, 2*6*dy, dy);
	}
}

static void draw_schematics (simulator_t *sim)
{
	GtkWidget *parent;
	GdkPixmap *pixmap;
	GdkColor yellow;
	int width, height, i;

	parent = gtk_widget_get_parent (GTK_WIDGET(sim->schematics));
	width = parent->allocation.width;
	height = parent->allocation.height;

	pixmap = gdk_pixmap_new (parent->window, width, height, -1);

	/* Жёлтый цвет. */
	sim->gc_yellow = gdk_gc_new (GDK_DRAWABLE (pixmap));
	gdk_gc_copy (sim->gc_yellow, parent->style->white_gc);
	yellow.red = 0xffff;
	yellow.green = 0xefff;
	yellow.blue = 0;
	gdk_gc_set_rgb_fg_color (sim->gc_yellow, &yellow);

	/* Фон. */
	gdk_draw_rectangle (pixmap, parent->style->bg_gc [GTK_STATE_NORMAL], TRUE,
		0, 0, width, height);

	/* Разбиваем область на 6 уровней по вертикали и на 2N+1 по горизонтали.
	 * Центрируем. */
	sim->dy = height / 6;
	sim->dx = width / (2*NNODES + 1);
	if (sim->dx > 3 * sim->dy)
		sim->dx = 3 * sim->dy;
	if (sim->dy > sim->dx / 2)
		sim->dy = sim->dx / 2;
	sim->node_left[0] = width/2 - sim->dx * 3/2;
	sim->node_left[1] = width/2 + sim->dx * 1/2;
	sim->node_center[0] = sim->node_left[0] + sim->dx/2;
	sim->node_center[1] = sim->node_left[1] + sim->dx/2;
	sim->cpu_top = height/2 - sim->dy * 5/2;
	sim->ctlr_top = height/2 - sim->dy * 1/2;
	sim->bus_top = height/2 + sim->dy * 3/2;

	/* Процессоры. */
	gdk_draw_rectangle (pixmap, parent->style->white_gc, TRUE,
		sim->node_left[0], sim->cpu_top, sim->dx, sim->dy);
	gdk_draw_rectangle (pixmap, parent->style->black_gc, FALSE,
		sim->node_left[0], sim->cpu_top, sim->dx, sim->dy);
	gdk_draw_rectangle (pixmap, parent->style->white_gc, TRUE,
		sim->node_left[1], sim->cpu_top, sim->dx, sim->dy);
	gdk_draw_rectangle (pixmap, parent->style->black_gc, FALSE,
		sim->node_left[1], sim->cpu_top, sim->dx, sim->dy);

	/* Контроллеры. */
	gdk_draw_rectangle (pixmap, parent->style->white_gc, TRUE,
		sim->node_left[0], sim->ctlr_top, sim->dx, sim->dy);
	gdk_draw_rectangle (pixmap, parent->style->black_gc, FALSE,
		sim->node_left[0], sim->ctlr_top, sim->dx, sim->dy);
	gdk_draw_rectangle (pixmap, parent->style->white_gc, TRUE,
		sim->node_left[1], sim->ctlr_top, sim->dx, sim->dy);
	gdk_draw_rectangle (pixmap, parent->style->black_gc, FALSE,
		sim->node_left[1], sim->ctlr_top, sim->dx, sim->dy);

	/* Шины. */
	gdk_draw_line (pixmap, parent->style->black_gc,
		sim->node_left[0], sim->bus_top, sim->node_left[1] + sim->dx, sim->bus_top);
	gdk_draw_line (pixmap, parent->style->black_gc,
		sim->node_left[0], sim->bus_top + sim->dy/2, sim->node_left[1] + sim->dx, sim->bus_top + sim->dy/2);

	/* Между процессорами и контроллерами. */
	gdk_draw_line (pixmap, parent->style->black_gc,
		sim->node_center[0], sim->cpu_top + sim->dy, sim->node_center[0], sim->ctlr_top);
	gdk_draw_line (pixmap, parent->style->black_gc,
		sim->node_center[1], sim->cpu_top + sim->dy, sim->node_center[1], sim->ctlr_top);

	/* Между контроллерами и шинами. */
	gdk_draw_line (pixmap, parent->style->black_gc,
		sim->node_center[0] - sim->dx/4, sim->ctlr_top + sim->dy, sim->node_center[0] - sim->dx/4, sim->bus_top);
	gdk_draw_line (pixmap, parent->style->black_gc,
		sim->node_center[0] + sim->dx/4, sim->ctlr_top + sim->dy, sim->node_center[0] + sim->dx/4, sim->bus_top + sim->dy/2);
	gdk_draw_line (pixmap, parent->style->black_gc,
		sim->node_center[1] - sim->dx/4, sim->ctlr_top + sim->dy, sim->node_center[1] - sim->dx/4, sim->bus_top);
	gdk_draw_line (pixmap, parent->style->black_gc,
		sim->node_center[1] + sim->dx/4, sim->ctlr_top + sim->dy, sim->node_center[1] + sim->dx/4, sim->bus_top + sim->dy/2);

	gtk_image_set_from_pixmap (sim->schematics, pixmap, 0);

	/* Двигаем кнопки. */
	gtk_fixed_move (GTK_FIXED (parent), GTK_WIDGET (sim->bus_combobox),
		sim->node_left[0] - 94, sim->bus_top + (sim->dy/2 - 31) / 2);
	for (i=0; i<NNODES; ++i) {
		gtk_fixed_move (GTK_FIXED (parent), GTK_WIDGET (sim->cpu_combobox[i]),
			sim->node_left[i] + (sim->dx - 90) / 2, sim->cpu_top + (sim->dy - 31) / 2);
		gtk_fixed_move (GTK_FIXED (parent), GTK_WIDGET (sim->ctlr_combobox[i]),
			sim->node_left[i] + (sim->dx - 90) / 2, sim->ctlr_top + (sim->dy - 31) / 2);
		gtk_fixed_move (GTK_FIXED (parent), GTK_WIDGET (sim->cpu_reset_checkbutton[i]),
			sim->node_left[i] + (sim->dx - 90) / 2, sim->cpu_top - 24);
	}
}

static void ui_step (simulator_t *sim)
{
	simulator_step (sim);
	chart_step (sim);
	append_history (sim);
	status_print (sim, "Шаг %d, время %.5f мксек", sim->nstep, sim->time_usec);
}

/*
 * Установка обработчика событий пользовательского интерфейса.
 */
static void signal_connect (simulator_t *sim, char *widget_name, char *signal_name,
	GCallback func)
{
	GtkWidget *widget;

	widget = glade_xml_get_widget (sim->ui, widget_name);
	g_signal_connect (G_OBJECT (widget), signal_name, func, sim);
}

/*
 * Кнопка завершения программы.
 */
static gboolean main_quit (GtkWidget *widget, simulator_t *sim)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_widget_get_toplevel (widget)),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
		"Завершить симулятор?");
	gtk_window_set_title (GTK_WINDOW (dialog), "Выход");
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES)
		gtk_main_quit ();
	gtk_widget_destroy (dialog);
	return TRUE;
}

static gboolean run_timer (simulator_t *sim)
{
	if (! sim->run_continuously)
		return FALSE;
	ui_step (sim);
	return TRUE;
}

/*
 * Пуск симулятора в непрерывном режиме.
 */
static gboolean button_run (GtkWidget *widget, simulator_t *sim)
{
	if (sim->run_continuously)
		return FALSE;
	g_timeout_add (10, (GSourceFunc) run_timer, (gpointer) sim);
	sim->run_continuously = 1;
	return TRUE;
}

/*
 * Останов симулятора, отмена непрерывного режима.
 */
static gboolean button_halt (GtkWidget *widget, simulator_t *sim)
{
	sim->run_continuously = 0;
	return TRUE;
}

/*
 * Один шаг симулятора.
 */
static gboolean button_step (GtkWidget *widget, simulator_t *sim)
{
	if (sim->run_continuously)
		return FALSE;
	ui_step (sim);
	return TRUE;
}

/*
 * Начинаем моделирование заново.
 */
static gboolean button_reset (GtkWidget *widget, simulator_t *sim)
{
	sim->run_continuously = 0;
	printf ("Simulator reset.\n");
	status_print (sim, "Simulator reset");
	simulator_reset (sim);
	clear_history (sim);
	chart_clear (sim);
	gtk_text_buffer_set_text (gtk_text_view_get_buffer (textview_log), "", -1);
	return TRUE;
}

static gboolean combobox_bus (GtkWidget *widget, simulator_t *sim)
{
	static const int bus_combobox_to_mbps [] = { 1, 2, 5, 10, 100 };
	int active;

	active = gtk_combo_box_get_active (sim->bus_combobox);
	simulator_set_bus_rate (sim, bus_combobox_to_mbps [active]);
	return TRUE;
}

static gboolean combobox_cpu (GtkWidget *widget, simulator_t *sim)
{
	static const int cpu_combobox_to_mhz [] = { 100, 50, 20, 10 };
	int i, active;

	for (i=0; i<NNODES; ++i) {
		if (widget == GTK_WIDGET (sim->cpu_combobox[i])) {
			active = gtk_combo_box_get_active (sim->cpu_combobox[i]);
			simulator_set_cpu_frequency (sim, i, cpu_combobox_to_mhz [active]);
			break;
		}
	}
	return TRUE;
}

static gboolean combobox_ctlr (GtkWidget *widget, simulator_t *sim)
{
	static const int ctlr_combobox_to_ppm [] =
		{ -1000, -500, -100, 0, 100, 500, 1000 };
	int i, active;

	for (i=0; i<NNODES; ++i) {
		if (widget == GTK_WIDGET (sim->ctlr_combobox[i])) {
			active = gtk_combo_box_get_active (sim->ctlr_combobox[i]);
			simulator_set_ctlr_frequency (sim, i,
				ctlr_combobox_to_ppm [active]);
			break;
		}
	}
	return TRUE;
}

static gboolean checkbutton_reset (GtkWidget *widget, simulator_t *sim)
{
	int i;

	for (i=0; i<NNODES; ++i) {
		if (widget == GTK_WIDGET (sim->cpu_reset_checkbutton[i])) {
			if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
				sim->rst[i] = 1;
				status_print (sim, "CPU %d reset", i);
			} else {
				sim->rst[i] = 0;
				status_print (sim, "CPU %d enable", i);
			}
			break;
		}
	}
	return TRUE;
}

/*
 * Отрисовка диаграммы.
 */
static gboolean chart_expose (GtkWidget *widget, GdkEventExpose *event,
	simulator_t *sim)
{
	int area_x, area_y, area_width, area_height;

	if (sim->chart_needs_redraw ||
	    (event->area.width == widget->allocation.width &&
	    event->area.height == widget->allocation.height)) {
		/* Полная перерисовка. */
		area_x = 0;
		area_y = 0;
		area_width = widget->allocation.width;
		area_height = widget->allocation.height;
	} else {
		/* Частичная перерисовка. */
		area_x = event->area.x;
		area_y = event->area.y;
		area_width = event->area.width;
		area_height = event->area.height;
	}
	/*printf ("Chart expose: %dx%d at %d,%d\n",
		area_width, area_height, area_x, area_y);*/

	chart_draw (sim, area_x, area_y, area_width, area_height);

	sim->chart_needs_redraw = 0;
	return TRUE;
}

/*
 * Диаграмма инициализирована, можно выделять ресурсы.
 */
static gboolean chart_realize (GtkWidget *widget, simulator_t *sim)
{
	static gint8 dashes [] = { 1, 1 };
	GdkColor color;

	/*printf ("Chart realize: %dx%d\n",
		widget->allocation.width, widget->allocation.height);*/
	/* Пунктирная линия. */
	sim->gc_dotted = gdk_gc_new (widget->window);
	gdk_gc_copy (sim->gc_dotted, widget->style->bg_gc [GTK_STATE_NORMAL]);
	gdk_gc_set_line_attributes (sim->gc_dotted, 1, GDK_LINE_ON_OFF_DASH,
		GDK_CAP_BUTT, GDK_JOIN_MITER);
	gdk_gc_set_dashes (sim->gc_dotted, 0, dashes, 2);

	/* Тёмно-серый цвет. */
	color.red = color.green = color.blue = 0x6000;
	gdk_gc_set_rgb_fg_color (sim->gc_dotted, &color);
	return TRUE;
}

/*
 * Изменился размер диаграммы.
 */
static gboolean chart_configure (GtkWidget *widget, GdkEventConfigure *event,
	simulator_t *sim)
{
	sim->chart_needs_redraw = 1;
	return TRUE;
}

/*
 * Инициализация интерфейсных элементов.
 */
static void ui_init (simulator_t *sim)
{
	GtkTextBuffer *buffer;
	GtkTextIter end_iter;

	/* Подготовка лога, создание метки "end". */
	textview_log = (GtkTextView*) glade_xml_get_widget (sim->ui, "textview_log");
	buffer = gtk_text_view_get_buffer (textview_log);
	gtk_text_buffer_get_end_iter (buffer, &end_iter);
	gtk_text_buffer_create_mark (buffer, "end", &end_iter, FALSE);

	/* Инициализация интерфейсных элементов. */
	sim->schematics = (GtkImage*) glade_xml_get_widget (sim->ui, "image_schematics");
	sim->statusbar = (GtkStatusbar*) glade_xml_get_widget (sim->ui, "statusbar");
	sim->chart = (GtkDrawingArea*) glade_xml_get_widget (sim->ui, "drawingarea_chart");

	sim->bus_combobox = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_bus");
	gtk_combo_box_set_active (sim->bus_combobox, 3);	/* 10 Мбит/сек */
	simulator_set_bus_rate (sim, 10);

	sim->cpu_combobox[0] = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_cpu0");
	gtk_combo_box_set_active (sim->cpu_combobox[0], 1);	/* 50 МГц */
	simulator_set_cpu_frequency (sim, 0, 50);

	sim->ctlr_combobox[0] = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_ctlr0");
	gtk_combo_box_set_active (sim->ctlr_combobox[0], 4);	/* +100 ppm */
	simulator_set_ctlr_frequency (sim, 0, 100);
	sim->cpu_reset_checkbutton[0] = (GtkCheckButton*)
		glade_xml_get_widget (sim->ui, "checkbutton_reset0");

	sim->cpu_combobox[1] = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_cpu1");
	gtk_combo_box_set_active (sim->cpu_combobox[1], 2);	/* 20 МГц */
	simulator_set_cpu_frequency (sim, 1, 20);

	sim->ctlr_combobox[1] = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_ctlr1");
	gtk_combo_box_set_active (sim->ctlr_combobox[1], 2);	/* -100 ppm */
	simulator_set_ctlr_frequency (sim, 1, -100);
	sim->cpu_reset_checkbutton[1] = (GtkCheckButton*)
		glade_xml_get_widget (sim->ui, "checkbutton_reset1");
#if NNODES > 2
	sim->cpu_combobox[2] = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_cpu2");
	gtk_combo_box_set_active (sim->cpu_combobox[2], 1);	/* 50 МГц */
	simulator_set_cpu_frequency (sim, 2, 50);

	sim->ctlr_combobox[2] = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_ctlr2");
	gtk_combo_box_set_active (sim->ctlr_combobox[2], 3);	/* 0 ppm */
	simulator_set_ctlr_frequency (sim, 2, 0);
	sim->cpu_reset_checkbutton[2] = (GtkCheckButton*)
		glade_xml_get_widget (sim->ui, "checkbutton_reset2");
#endif
#if NNODES > 3
	sim->cpu_combobox[3] = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_cpu3");
	gtk_combo_box_set_active (sim->cpu_combobox[3], 1);	/* 50 МГц */
	simulator_set_cpu_frequency (sim, 3, 50);

	sim->ctlr_combobox[3] = (GtkComboBox*)
		glade_xml_get_widget (sim->ui, "combobox_ctlr3");
	gtk_combo_box_set_active (sim->ctlr_combobox[3], 3);	/* 0 ppm */
	simulator_set_ctlr_frequency (sim, 3, 0);
	sim->cpu_reset_checkbutton[3] = (GtkCheckButton*)
		glade_xml_get_widget (sim->ui, "checkbutton_reset3");
#endif
	/* Устанавливаем обработчики событий пользовательского интерфейса. */
	signal_connect (sim, "window_top", "delete_event", G_CALLBACK (main_quit));
	signal_connect (sim, "drawingarea_chart", "configure_event", G_CALLBACK (chart_configure));
	signal_connect (sim, "drawingarea_chart", "expose_event", G_CALLBACK (chart_expose));
	signal_connect (sim, "drawingarea_chart", "realize", G_CALLBACK (chart_realize));
	signal_connect (sim, "button_quit", "clicked", G_CALLBACK (main_quit));
	signal_connect (sim, "button_run", "clicked", G_CALLBACK (button_run));
	signal_connect (sim, "button_halt", "clicked", G_CALLBACK (button_halt));
	signal_connect (sim, "button_step", "clicked", G_CALLBACK (button_step));
	signal_connect (sim, "button_reset", "clicked", G_CALLBACK (button_reset));
	signal_connect (sim, "combobox_bus", "changed", G_CALLBACK (combobox_bus));
	signal_connect (sim, "combobox_cpu0", "changed", G_CALLBACK (combobox_cpu));
	signal_connect (sim, "combobox_ctlr0", "changed", G_CALLBACK (combobox_ctlr));
	signal_connect (sim, "checkbutton_reset0", "toggled", G_CALLBACK (checkbutton_reset));
	signal_connect (sim, "combobox_cpu1", "changed", G_CALLBACK (combobox_cpu));
	signal_connect (sim, "combobox_ctlr1", "changed", G_CALLBACK (combobox_ctlr));
	signal_connect (sim, "checkbutton_reset1", "toggled", G_CALLBACK (checkbutton_reset));
#if NNODES > 2
	signal_connect (sim, "combobox_cpu2", "changed", G_CALLBACK (combobox_cpu));
	signal_connect (sim, "combobox_ctlr2", "changed", G_CALLBACK (combobox_ctlr));
	signal_connect (sim, "checkbutton_reset2", "toggled", G_CALLBACK (checkbutton_reset));
#endif
#if NNODES > 3
	signal_connect (sim, "combobox_cpu3", "changed", G_CALLBACK (combobox_cpu));
	signal_connect (sim, "combobox_ctlr3", "changed", G_CALLBACK (combobox_ctlr));
	signal_connect (sim, "checkbutton_reset3", "toggled", G_CALLBACK (checkbutton_reset));
#endif
}

int main (int argc, char *argv[])
{
	simulator_t *sim;

	sim = calloc (1, sizeof (*sim));
	if (! sim) {
		fprintf (stderr, "Out of memory\n");
		exit (1);
	}
	gtk_init (&argc, &argv);

	/* Читаем структуру пользовательского интерфейса из XML-файла glade. */
	sim->ui = glade_xml_new ("simulator.glade", "window_top", NULL);

	ui_init (sim);
	simulator_init (sim);
	clear_history (sim);
	draw_schematics (sim);

	printf ("Started.\n");
	status_print (sim, "Started");

	gtk_main();
	return 0;
}
