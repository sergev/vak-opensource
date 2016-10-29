# include "jlib.h"

extern curgroup, curwin;

help ()
{
	Jmove (3, 0);
	Jprintf ("\tHelp или Справка - краткая справка по командам\n\n\r");
	Jprintf ("\n\t*\t- шаг\n\r");
	Jprintf ("\n\t/\t- переключение цветовой палитры\n\r");
}