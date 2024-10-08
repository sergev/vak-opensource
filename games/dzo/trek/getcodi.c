# include       "getpar.h"

/*
**  get course and distance
**
**      The user is asked for a course and distance.  This is used by
**      move, impulse, and some of the computer functions.
**
**      The return value is zero for success, one for an invalid input
**      (meaning to drop the request).
*/

getcodi(co, di)
int     *co;
FLOAT   *di;
{

	*co = getintpar("����");

	/* course must be in the interval [0, 360] */
	if (*co < 0 || *co > 360)
		return (1);
	*di = getfltpar("����������");

	/* distance must be in the interval [0, 15] */
	if (*di <= 0.0 || *di > 15.0)
		return (1);

	/* good return */
	return (0);
}
