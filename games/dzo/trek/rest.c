# include       "trek.h"
# include       "getpar.h"

/*
**  REST FOR REPAIRS
**
**      You sit around and wait for repairs to happen.  Actually, you
**      sit around and wait for anything to happen.  I do want to point
**      out however, that Klingons are not as patient as you are, and
**      they tend to attack you while you are resting.
**
**      You can never rest through a long range tractor beam.
**
**      In events() you will be given an opportunity to cancel the
**      rest period if anything momentous happens.
*/

void rest(int _)
{
	FLOAT                   t;
	register int            percent;

	/* get the time to rest */
	t = getfltpar("Как долго");
	if (t <= 0.0)
		return;
	percent = 100 * t / Now.time + 0.5;
	if (percent >= 70)
	{
		printf("Спок: На это уйдет %d%% оставшегося времени.\n",
			percent);
		if (!getynpar("Будете ли Вы дрейфовать"))
			return;
	}
	Move.time = t;

	/* boundary condition is the LRTB */
	t = Now.eventptr[E_LRTB]->date - Now.date;
	if (Ship.cond != DOCKED && Move.time > t)
		Move.time = t + 0.0001;
	Move.free = 0;
	Move.resting = 1;
}
