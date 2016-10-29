/*
 * Простейшее окно: одно текстовое поле и одна кнопка.
 */
#include <QWidget>

class QTextEdit;
class QPushButton;

class Hello : public QWidget
{
	Q_OBJECT

public:
	Hello (QWidget *parent = 0);

public slots:
	void button_clicked();

private:
	QTextEdit *text;
	QPushButton *button;
};
