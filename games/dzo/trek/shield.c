# include       "trek.h"
# include       "getpar.h"

/*
**  SHIELD AND CLOAKING DEVICE CONTROL
**
**      'f' is one for auto shield up (in case of Condition RED),
**      zero for shield control, and negative one for cloaking
**      device control.
**
**      Called with an 'up' or 'down' on the same line, it puts
**      the shields/cloak into the specified mode.  Otherwise it
**      reports to the user the current mode, and asks if she wishes
**      to change.
**
**      This is not a free move.  Hits that occur as a result of
**      this move appear as though the shields are half up/down,
**      so you get partial hits.
*/

struct cvntab Udtab[] =
{
     {  "u",    "p",            1,  0,  0 },
     {  "вкл",  "ючить",        1,  0,  0 },
     {  "d",    "own",          0,  0,  0 },
     {  "выкл", "ючить",        0,  0,  0 },
     {  0,       0,             0,  0,  0 }
};

void shield(f)
int     f;
{
	register int            i;
	char                    c;
	struct cvntab           *r;
	char                    s[100];
	char                    *device;
	int                     ind;
	char                    *stat;

	if (f > 0 && (Ship.shldup || damaged(SRSCAN)))
		return;
	if (f < 0)
	{
		/* cloaking device */
		if (Ship.ship == QUEENE) {
			printf("Faire Queene не может быть закрыта полем.\n");
			return;
			}
		device = "Устройство закрытия";
		ind = CLOAK;
		stat = &Ship.cloaked;
	}
	else
	{
		/* shields */
		device = "Защитное поле";
		ind = SHIELD;
		stat = &Ship.shldup;
	}
	if (damaged(ind))
	{
		if (f <= 0)
			out(ind);
		return;
	}
	if (Ship.cond == DOCKED)
	{
		printf("%s должно быть выключено при стыковке\n", device);
		return;
	}
	if (f <= 0 && !testnl())
	{
		r = getcodpar("Включить/выключить", Udtab);
		i = r->value;
	}
	else
	{
		if (*stat)
			sprintf(s, "%s включено.  Выключить", device);
		else
			sprintf(s, "%s выключено.  Включить", device);
		if (!getynpar(s))
			return;
		i = !*stat;
	}
	if (*stat == i)
	{
		printf("%s уже ", device);
		if (i)
			printf("включено\n");
		else
			printf("выключено\n");
		return;
	}
	if (i)
		if (f >= 0)
			Ship.energy -= Param.shupengy;
		else
			Ship.cloakgood = 0;
	Move.free = 0;
	if (f >= 0)
		Move.shldchg = 1;
	*stat = i;
	return;
}
