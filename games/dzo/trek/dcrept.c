#include "trek.h"

/*
**  damage control report
**
**      Print damages and time to fix.  This is taken from the event
**      list.  A couple of factors are set up, based on whether or not
**      we are docked.  (One of these factors will always be 1.0.)
**      The event list is then scanned for damage fix events, the
**      time until they occur is determined, and printed out.  The
**      magic number DAMFAC is used to tell how much faster you can
**      fix things if you are docked.
*/

void dcrept(int _)
{
    register int i, f;
    FLOAT x;
    FLOAT m1, m2;
    register struct event *e;

    /* set up the magic factors to output the time till fixed */
    if (Ship.cond == DOCKED) {
        m1 = 1.0 / Param.dockfac;
        m2 = 1.0;
    } else {
        m1 = 1.0;
        m2 = Param.dockfac;
    }
    printf("Информация о неисправностях:\n");
    f = 1;

    /* scan for damages */
    for (i = 0; i < MAXEVENTS; i++) {
        e = &Event[i];
        if (e->evcode != E_FIXDV)
            continue;

        /* output the title first time */
        if (f) {
            printf("\t\t\t  время ремонта\n");
            printf("в полете   на базе    устройство\n");
            f = 0;
        }

        /* compute time till fixed, then adjust by the magic factors */
        x = e->date - Now.date;
        printf("%7.2f  %7.2f  %s\n", x * m1 + 0.005, x * m2 + 0.005, Device[e->systemname].name);

        /* do a little consistancy checking */
    }

    /* if everything was ok, reassure the nervous captain */
    if (f)
        printf("Все устройства исправны\n");
}
