#include <QtGui>
#include "hello.h"

/*
 * Простая стандартная вызывалка.
 */
int main (int argc, char *argv[])
{
	QApplication app (argc, argv);

	Hello *hello = new Hello;
	hello->show();

	return app.exec();
}
