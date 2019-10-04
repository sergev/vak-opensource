#include <QtWidgets>
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    // Creates an instance of QApplication
    QApplication app(argc, argv);

    // This is our MainWidget class containing our GUI and functionality
    MainWidget w;
    w.show(); // Show main window

    // run the application and return execs() return value/code
    return app.exec();
}
