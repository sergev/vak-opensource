# ifndef NULL
#    define NULL ((char *) 0)
# endif

extern halting;
extern curgroup, curwin;

extern Jprintf ();
extern char *getstring ();

ctrl1 ()
{
	win0 ();
	if (getchoice (1, " У0ио ", "Обнулить процессор и игнорировать остановы ?",
	    NULL, " Да ", " Нет ", NULL) == 0) {
		send (0, 0363);
		zero ();
		halting = 1;
		win0 ();
	}
	curwin = 0;
	prmenu ();
}

ctrl2 ()
{
	win0 ();
	if (getchoice (1, " У0 ", "Обнулить процессор ?",
	    NULL, " Да ", " Нет ", NULL) == 0) {
		zero ();
		halting = 1;
		win0 ();
	}
	curwin = 0;
	prmenu ();
}

ctrl3 ()
{
	win0 ();
	if (getchoice (1, " ОР ", "Остановить процессор ?",
	    NULL, " Да ", " Нет ", NULL) == 0) {
		stop ();
		halting = 1;
		win0 ();
	}
	curwin = 0;
	prmenu ();
}

ctrl4 ()
{
	win0 ();
	if (getchoice (1, " АР ", "Пустить процессор ?",
	    NULL, " Да ", " Нет ", NULL) == 0) {
		run ();
		halting = 0;
		win0 ();
	}
	curwin = 0;
	prmenu ();
}

ctrl5 ()
{
	win0 ();
	if (getchoice (1, " ПктАр ", "Пустить процессор с команды на РКП ?",
	    NULL, " Да ", " Нет ", NULL) == 0) {
		setcrp ();
		run ();
		halting = 0;
		win0 ();
	}
	curwin = 0;
	prmenu ();
}

ctrl6 ()
{
	register char *p;
	char a [40+1];
	static long crp;

	sprintf (a, "%08lx", crp);
	p = getstring (8, a, " Пкт ", "Введите шестнадцатеричную команду");
	if (p) {
		sscanf (p, "%lx", &crp);
		ptsendl (crp, 0371);
		win0 ();
	}
	curwin = 0;
	prmenu ();
}

ctrl7 ()
{
	register char *p;
	static char name [40+1];

	p = getstring (40, name, " Загрузка ", "Введите имя файла");
	if (! p)
		goto ret;
	strncpy (name, p, 40);
	name [40] = 0;
	Jmove (12, 10);
	Jbold ();
	if (loadfile (name, Jprintf) < 0)
		error ("Не могу открыть '%s'", name);
	Jnorm ();
ret:
	curwin = 0;
	prmenu ();
}

ctrl8 ()
{
	notyet ();
}
