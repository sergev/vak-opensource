#include <QtGui>
#include "hello.h"

Hello::Hello (QWidget *parent)
	: QWidget (parent)
{
	/* Создаём поле с текстом для сообщений программы. */
	text = new QTextEdit;
	text->setReadOnly (true);
	text->setPlainText ("");

	/* Создаём кнопку и подключаем к ней сигнал. */
	button = new QPushButton (tr ("&Hello"));
	button->show();
	connect (button, SIGNAL (clicked()), this, SLOT (button_clicked()));

	/* Определяем вертикальное размещение для кнопок. */
	QVBoxLayout *button_layout = new QVBoxLayout;
	button_layout->addWidget (button, Qt::AlignTop);
	button_layout->addStretch ();

	/* Определяем табличное размещение для текста и кнопок. */
	QGridLayout *layout = new QGridLayout;
	layout->addWidget (text, 0, 0);
	layout->addLayout (button_layout, 0, 1);

	/* Задаём размещение и титул главного окна. */
	setLayout (layout);
	setWindowTitle (tr ("Hello"));
}

/*
 * Нажата кнопка - выдаём сообщение.
 */
void Hello::button_clicked ()
{
	text->append (tr ("Hello, World!"));
}
