#include <QApplication>
#include <QTextCodec>
#include "diag.h"

int main (int argc, char *argv[])
{
	QApplication app (argc, argv);
	QTextCodec::setCodecForTr (QTextCodec::codecForName ("utf8"));

	TopWindow top_window;
	top_window.show ();

	return app.exec ();
}
