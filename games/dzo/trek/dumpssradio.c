# include       "trek.h"

/**
 **     output hidden distress calls
 **/

dumpssradio()
{
	register struct event   *e;
	register int            j;
	register int            chkrest;

	chkrest = 0;
	for (j = 0; j < MAXEVENTS; j++)
	{
		e = &Event[j];
		/* if it is not hidden, then just ignore it */
		if ((e->evcode & E_HIDDEN) == 0)
			continue;
		if (e->evcode & E_GHOST)
		{
			unschedule(e);
			printf("Звездная система %s в квадранте %d,%d уже в безопасности\n",
				systemname(e), e->x, e->y);
			continue;
		}

		switch (e->evcode)
		{

		  case E_KDESB:
			printf("Звездная база в квадранте %d,%d атакована клингами\n",
				e->x, e->y);
			chkrest++;
			break;

		  case E_ENSLV:
		  case E_REPRO:
			printf("Звездной системе %s в квадранте %d,%d угрожает опасность\n",
				systemname(e), e->x, e->y);
			chkrest++;
			break;

		}
	}

	return (chkrest);
}
