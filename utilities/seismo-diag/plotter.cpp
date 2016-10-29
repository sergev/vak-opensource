#include <QtGui>
#include "plotter.h"

Plotter::Plotter (QWidget *parent)
	: QWidget (parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize Plotter::sizeHint() const
{
	return QSize (500, 150);
}

void Plotter::paintEvent (QPaintEvent * /*event*/)
{
	//printf ("paint event()\n");
	QPainter paint;
        paint.begin (this);
	paint.drawPixmap (0, 0, pixmap);
	paint.end();
}

void Plotter::resizeEvent (QResizeEvent * /*event*/)
{
	//printf ("resize event() width=%d, height=%d\n", width(), height());
	pixmap = QPixmap (size());
	pixmap.fill (Qt::lightGray);
}

/*
 * Рисуем одно измерение из пакета.
 * Всего в пакете packet_length измерений.
 * Они поступают по одному с номерами n от 0 и дальше.
 */
void Plotter::add_sample (int percent, int n, int packet_length)
{
	if (n == 0) {
		/* Прокрутка окна. */
//		pixmap.scroll (-packet_length, 0, rect());
		QPainter painter;
		painter.begin (&pixmap);
		painter.drawPixmap (-packet_length, 0, pixmap);
		painter.fillRect (width() - packet_length, 0,
			packet_length, height(), Qt::lightGray);
		painter.end();
	}

	int h = -percent * height() / 200;
	int y = height() / 2;
	//printf ("%d >> %d %d\n", percent, y, h);

	QPainter painter;
	painter.begin (&pixmap);
	if (h > 0)
		painter.fillRect (width() - packet_length + n, y, 1, h, Qt::darkGreen);
	else if (h < 0)
		painter.fillRect (width() - packet_length + n, y+h, 1, -h, Qt::darkGreen);
	painter.end();
	update ();
}

/*
 * Рисуем ежесекундную метку времени.
 */
void Plotter::draw_label (int n, int packet_length)
{
	QPainter painter;
	painter.begin (&pixmap);
	painter.fillRect (width() - packet_length + n, 0, 1, height(), Qt::yellow);
	painter.end();
	update ();
}

/*
 * Рисуем время начала пакета.
 */
void Plotter::draw_time (double sec)
{
	int min = sec / 60;
	sec -= min * 60;

	QString message;
	message.sprintf ("%d:%02d:%02.3f", min / 60, min % 60, sec);

	QPainter painter;
	painter.begin (&pixmap);
	painter.fillRect (width() - 1, 0, 1, height(), Qt::darkRed);

	painter.drawText (0, 2, width() - 2, height(),
		Qt::AlignRight | Qt::AlignTop, message);
	painter.end();
	update ();
}
