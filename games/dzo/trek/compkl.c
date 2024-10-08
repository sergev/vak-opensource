#include "trek.h"

/*
**  sort klingons
**
**      bubble sort on ascending distance
*/

void sortkl()
{
    struct kling t;
    register int f, i, m;

    m = Etc.nkling - 1;
    f = 1;
    while (f) {
        f = 0;
        for (i = 0; i < m; i++)
            if (Etc.klingon[i].dist > Etc.klingon[i + 1].dist) {
                bmove((char *)&Etc.klingon[i], (char *)&t, sizeof t);
                bmove((char *)&Etc.klingon[i + 1], (char *)&Etc.klingon[i], sizeof t);
                bmove((char *)&t, (char *)&Etc.klingon[i + 1], sizeof t);
                f = 1;
            }
    }
}

/*
**  compute klingon distances
**
**      The klingon list has the distances for all klingons recomputed
**      and sorted.  The parameter is a Boolean flag which is set if
**      we have just entered a new quadrant.
**
**      This routine is used every time the Enterprise or the Klingons
**      move.
*/

void compkldist(int f) /* set if new quadrant */
{
    register int i, dx, dy;
    FLOAT d;
    FLOAT temp;

    if (Etc.nkling == 0)
        return;
    for (i = 0; i < Etc.nkling; i++) {
        /* compute distance to the Klingon */
        dx = Ship.sectx - Etc.klingon[i].x;
        dy = Ship.secty - Etc.klingon[i].y;
        d  = dx * dx + dy * dy;
        d  = sqrt(d);

        /* compute average of new and old distances to Klingon */
        if (!f) {
            temp                   = Etc.klingon[i].dist;
            Etc.klingon[i].avgdist = 0.5 * (temp + d);
        } else {
            /* new quadrant: average is current */
            Etc.klingon[i].avgdist = d;
        }
        Etc.klingon[i].dist = d;
    }

    /* leave them sorted */
    sortkl();
}
