# include       "trek.h"
# include       "getpar.h"

/*
**  SET WARP FACTOR
**
**      The warp factor is set for future move commands.  It is
**      checked for consistancy.
*/

void setwarp()
{
	FLOAT   warpfac;

	warpfac = getfltpar("Скорость");
	if (warpfac < 0.0)
		return;
	if (warpfac < 1.0) {
		printf("Минимальная скорость 1.0\n");
		return;
		}
	if (warpfac > 10.0) {
		printf("Максимальная скорость 10.0\n");
		return;
		}
	if (warpfac > 6.0)
		printf("При скорости больше 6.0 могут сломаться двигатели\n");
	Ship.warp = warpfac;
	Ship.warp2 = Ship.warp * warpfac;
	Ship.warp3 = Ship.warp2 * warpfac;
}
