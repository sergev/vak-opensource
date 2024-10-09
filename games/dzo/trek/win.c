#include "getpar.h"
#include "trek.h"

/*
**  Signal game won
**
**      This routine prints out the win message, arranges to print out
**      your score, tells you if you have a promotion coming to you,
**      cleans up the current input line, and arranges to have you
**      asked whether or not you want another game (via the reset()
**      call).
**
**      Pretty straightforward, although the promotion algorithm is
**      pretty off the wall.
*/

void win()
{
    long s;
    extern struct cvntab Skitab[];
    register struct cvntab *p;

    sleep(1);
    printf("\nПоздравляем, Вы спасли Федерацию\n");
    Move.endgame = 1;

    /* print and return the score */
    s = score();

    /* decide if she gets a promotion */
    if (Game.helps == 0 && Game.killb == 0 && Game.killinhab == 0 &&
        5 * Game.kills + Game.deaths < 100 && s >= 1000 && Ship.ship == ENTERPRISE) {
        printf("Вы явно подняли свой класс,\n");
        if (Game.skill >= 6)
            printf("  Вам присвоено звание \"Commodore Emeritus\".\n");
        else {
            p = &Skitab[2 * Game.skill - 1];
            printf("с %s%s ", p->abrev, p->full);
            p += 2;
            printf("до %s%s.\n", p->abrev, p->full);
        }
    }

    /* clean out input, and request new game */
    skiptonl(0);
    reset(0);
}
