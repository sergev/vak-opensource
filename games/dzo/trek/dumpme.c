#include "trek.h"

/*
**  Dump the starship somewhere in the galaxy
**
**      Parameter is zero if bounce off of negative energy barrier,
**      one if through a black hole
**
**      Note that the quadrant is NOT initialized here.  This must
**      be done from the calling routine.
**
**      Repair of devices must be deferred.
*/

void dumpme(int flag)
{
    register int f;
    FLOAT x;
    register struct event *e;
    register int i;

    f          = flag;
    Ship.quadx = ranf(NQUADS);
    Ship.quady = ranf(NQUADS);
    Ship.sectx = ranf(NSECTS);
    Ship.secty = ranf(NSECTS);
    x += 1.5 * franf();
    Move.time += x;
    if (f) {
        printf("%s провалился в черную дыру.\n", Ship.shipname);
    } else {
        printf("Компьютер использовал всю мощность для предотвращения взрыва\n");
        printf("   барьера негативной энергии.  Была достигнута скорость света.\n");
    }
    /* bump repair dates forward */
    for (i = 0; i < MAXEVENTS; i++) {
        e = &Event[i];
        if (e->evcode != E_FIXDV)
            continue;
        reschedule(e, (e->date - Now.date) + x);
    }
    events(1);
    printf("Вы в квадранте %d,%d.  Звездное время %.2f\n", Ship.quadx, Ship.quady, Now.date);
    Move.time = 0;
}
