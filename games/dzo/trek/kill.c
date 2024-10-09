# include       "trek.h"

/*
**  KILL KILL KILL !!!
**
**      This file handles the killing off of almost anything.
*/

/*
**  Handle a Klingon's death
**
**      The Klingon at the sector given by the parameters is killed
**      and removed from the Klingon list.  Notice that it is not
**      removed from the event list; this is done later, when the
**      the event is to be caught.  Also, the time left is recomputed,
**      and the game is won if that was the last klingon.
*/

void killk(int ix, int iy)
{
	register int            i, j;

	printf("   *** Клинг в %d,%d уничтожен ***\n", ix, iy);

	/* remove the scoundrel */
	Now.klings -= 1;
	Sect[ix][iy] = EMPTY;
	Quad[Ship.quadx][Ship.quady].klings -= 1;
	/* %%% IS THIS SAFE???? %%% */
	Quad[Ship.quadx][Ship.quady].scanned -= 100;
	Game.killk += 1;

	/* find the Klingon in the Klingon list */
	for (i = 0; i < Etc.nkling; i++)
		if (ix == Etc.klingon[i].x && iy == Etc.klingon[i].y)
		{
			/* purge him from the list */
			Etc.nkling -= 1;
			for (; i < Etc.nkling; i++)
				bmove((char*) &Etc.klingon[i+1], (char*) &Etc.klingon[i], sizeof Etc.klingon[i]);
			break;
		}

	/* find out if that was the last one */
	if (Now.klings <= 0)
		win();

	/* recompute time left */
	Now.time = Now.resource / Now.klings;
	return;
}


/*
**  handle a starbase's death
*/

void killb(int qx, int qy)
{
	register struct quad    *q;
	register struct xy      *b;

	q = &Quad[qx][qy];

	if (q->bases <= 0)
		return;
	if (!damaged(SSRADIO)) {
		/* then update starchart */
		if (q->scanned < 1000) {
			q->scanned -= 10;
		} else {
			if (q->scanned > 1000)
				q->scanned = -1;
                }
        }
	q->bases = 0;
	Now.bases -= 1;
	for (b = Now.base; ; b++)
		if (qx == b->x && qy == b->y)
			break;
	bmove((char*) &Now.base[Now.bases], (char*) b, sizeof *b);
	if (qx == Ship.quadx && qy == Ship.quady)
	{
		Sect[Etc.starbase.x][Etc.starbase.y] = EMPTY;
		if (Ship.cond == DOCKED)
			undock(0);
		printf("Звездная база в %d,%d уничтожена\n", Etc.starbase.x, Etc.starbase.y);
	}
	else
	{
		if (!damaged(SSRADIO))
		{
			printf("Ухура: Командование звездного флота сообщило, что\n");
			printf("  звездная база в квадранте %d,%d уничтожена\n", qx, qy);
		}
		else
			schedule(E_KATSB | E_GHOST, 1e50, qx, qy, 0);
	}
}


/**
 **     kill an inhabited starsystem
 **/

void kills(int x, int y,    /* quad coords if f == 0, else sector coords */
           int f)           /* f != 0 -- this quad;  f < 0 -- Enterprise's fault */
{
	register struct quad    *q;
	register struct event   *e;
	register char           *name;

	if (f)
	{
		/* current quadrant */
		q = &Quad[Ship.quadx][Ship.quady];
		Sect[x][y] = EMPTY;
		name = systemname(q);
		if (name == 0)
			return;
		printf("Беззащитная звездная система %s в %d,%d разрушена\n",
			name, x, y);
		if (f < 0)
			Game.killinhab += 1;
	}
	else
	{
		/* different quadrant */
		q = &Quad[x][y];
	}
	if (q->qsystemname & Q_DISTRESSED)
	{
		/* distressed starsystem */
		e = &Event[q->qsystemname & Q_SYSTEM];
		printf("Сигнал бедствия для %s уже не нужен\n",
			Systemname[e->systemname]);
		unschedule(e);
	}
	q->qsystemname = 0;
	q->stars -= 1;
}


/**
 **     "kill" a distress call
 **/

void killd(int x, int y,    /* quadrant coordinates */
           int f)           /* set if user is to be informed */
{
	register struct event   *e;
	register int            i;
	register struct quad    *q;

	q = &Quad[x][y];
	for (i = 0; i < MAXEVENTS; i++)
	{
		e = &Event[i];
		if (e->x != x || e->y != y)
			continue;
		switch (e->evcode)
		{
		  case E_KDESB:
			if (f)
			{
				printf("Сообщение о бедственном положении звездной базы в %d,%d аннулировано\n",
					x, y);
				unschedule(e);
			}
			break;

		  case E_ENSLV:
		  case E_REPRO:
			if (f)
			{
				printf("Сообщение о бедственном положении %s в квадранте %d,%d аннулировано\n",
					Systemname[e->systemname], x, y);
				q->qsystemname = e->systemname;
				unschedule(e);
			}
			else
			{
				e->evcode |= E_GHOST;
			}
		}
	}
}
