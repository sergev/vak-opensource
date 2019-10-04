#include <QtWidgets>
#include "mainwidget.h"

// Constructor for main widget
MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent)
{
    button_ = new QPushButton(tr("Push Me!"));
    textBrowser_ = new QTextBrowser();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(button_,0,0);
    mainLayout->addWidget(textBrowser_,1,0);
    setLayout(mainLayout);
    setWindowTitle(tr("Connecting buttons to processes"));
}

// Destructor
MainWidget::~MainWidget()
{
    delete button_;
    delete textBrowser_;

    //TODO: Add buttons and indicators GP0, GP1, GP2, GP3, UTX, URX, SCL, SDA
}
