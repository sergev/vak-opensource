# include       "trek.h"
# include       "getpar.h"

/*
**  MOVE UNDER WARP POWER
**
**      This is both the "move" and the "ram" commands, differing
**      only in the flag 'fl'.  It is also used for automatic
**      emergency override mode, when 'fl' is < 0 and 'c' and 'd'
**      are the course and distance to be moved.  If 'fl' >= 0,
**      the course and distance are asked of the captain.
**
**      The guts of this routine are in the routine move(), which
**      is shared with impulse().  Also, the working part of this
**      routine is very small; the rest is to handle the slight chance
**      that you may be moving at some riduculous speed.  In that
**      case, there is code to handle time warps, etc.
*/

void warp1(int fl)
{
    warp(fl, 0, 0.0);
}

void warp(int fl, int c, FLOAT d)
{
	int                     course;
	FLOAT                   power;
	FLOAT                   dist;
	FLOAT                   time;
	FLOAT                   speed;
	FLOAT                   frac;
	register int            percent;
	register int            i;
	char                   *px;

	if (Ship.cond == DOCKED) {
		printf("%s стыкован с базой\n", Ship.shipname);
		return;
		}
	if (damaged(WARP))
	{
		out(WARP);
		return;
	}
	if (fl < 0)
	{
		course = c;
		dist = d;
	}
	else
		if (getcodi(&course, &dist))
			return;

	/* check to see that we are not using an absurd amount of power */
	power = (dist + 0.05) * Ship.warp3;
	percent = 100 * power / Ship.energy + 0.5;
	if (percent >= 85)
	{
		printf("Скотти: Это потребует %d%% оставшейся энергии.\n",
			percent);
		if (!getynpar("Продолжать"))
			return;
	}

	/* compute the speed we will move at, and the time it will take */
	speed = Ship.warp2 / Param.warptime;
	time = dist / speed;

	/* check to see that that value is not ridiculous */
	percent = 100 * time / Now.time + 0.5;
	if (percent >= 85)
	{
		printf("Спок: На это уйдет %d%% оставшегося времени.\n",
			percent);
		if (!getynpar("Делать это"))
			return;
	}

	/* compute how far we will go if we get damages */
	if (Ship.warp > 6.0 && ranf(100) < 20 + 15 * (Ship.warp - 6.0))
	{
		frac = franf();
		dist *= frac;
		time *= frac;
		damage(WARP, (frac + 1.0) * Ship.warp * (franf() + 0.25) * 0.20);
	}

	/* do the move */
	Move.time = move(fl, course, time, speed);

	/* see how far we actually went, and decrement energy appropriately */
	dist = Move.time * speed;
	Ship.energy -= dist * Ship.warp3 * (Ship.shldup + 1);

	/* test for bizarre events */
	if (Ship.warp <= 9.0)
		return;
	printf("\n\n  ___ Скорость выше 9.0 ___\n\n");
	sleep(2);
	printf("Безопасность корабля весьма сомнительна\n");
	sleep(2);
	printf("Команда испытывает крайне неприятные ощущения...\n");
	sleep(4);
	if (ranf(100) >= 100 * dist)
	{
		printf("Равновесие восстановлено -- все системы в порядке\n");
		return;
	}

	/* select a bizzare thing to happen to us */
	percent = ranf(100);
	if (percent < 70)
	{
		/* time warp */
		if (percent < 35 || !Game.snap)
		{
			/* positive time warp */
			time = (Ship.warp - 8.0) * dist * (franf() + 1.0);
			Now.date += time;
			printf("Положительный скачок во времени -- сейчас звездное время %.2f\n",
				Now.date);
			for (i = 0; i < MAXEVENTS; i++)
			{
				percent = Event[i].evcode;
				if (percent == E_FIXDV || percent == E_LRTB)
					Event[i].date += time;
			}
			return;
		}

		/* s/he got lucky: a negative time portal */
		time = Now.date;
		px = Etc.snapshot;
		bmove(px, (char*) Quad, sizeof Quad);
		bmove(px += sizeof Quad, (char*) Event, sizeof Event);
		bmove(px += sizeof Event, (char*) &Now, sizeof Now);
		printf("Негативный скачок во времени -- сейчас звездное время %.2f\n",
			Now.date);
		for (i = 0; i < MAXEVENTS; i++)
			if (Event[i].evcode == E_FIXDV)
				reschedule(&Event[i], Event[i].date - time);
		return;
	}

	/* test for just a lot of damage */
	if (percent < 80)
		lose(L_TOOFAST);
	printf("Равновесие восстановлено -- системы корабля сильно повреждены\n");
	for (i = 0; i < NDEV; i++)
		damage(i, (3.0 * (franf() + franf()) + 1.0) * Param.damfac[i]);
	Ship.shldup = 0;
}
