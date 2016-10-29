#ifndef PLOTTER_H
#define PLOTTER_H

#include <QWidget>
#include <QPixmap>

QT_BEGIN_NAMESPACE
class QWidget;
class QPixmap;
QT_END_NAMESPACE

class Plotter : public QWidget
{
	Q_OBJECT

public:
	Plotter (QWidget *parent = 0);
	void add_sample (int percent, int n, int packet_length);
	void draw_label (int n, int packet_length);
	void draw_time (double sec);

protected:
	void paintEvent (QPaintEvent *event);
	void resizeEvent (QResizeEvent *event);
	QSize sizeHint() const;

private:
	QPixmap pixmap;
};

#endif
