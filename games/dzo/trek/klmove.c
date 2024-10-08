#include "trek.h"

/*
**  Move Klingons Around
**
**      This is a largely incomprehensible block of code that moves
**      Klingons around in a quadrant.  It was written in a very
**      "program as you go" fashion, and is a prime candidate for
**      rewriting.
**
**      The flag `fl' is zero before an attack, one after an attack,
**      and two if you are leaving a quadrant.  This serves to
**      change the probability and distance that it moves.
**
**      Basically, what it will try to do is to move a certain number
**      of steps either toward you or away from you.  It will avoid
**      stars whenever possible.  Nextx and nexty are the next
**      sector to move to on a per-Klingon basis; they are roughly
**      equivalent to Ship.sectx and Ship.secty for the starship.  Lookx and
**      looky are the sector that you are going to look at to see
**      if you can move their.  Dx and dy are the increment.  Fudgex
**      and fudgey are the things you change around to change your
**      course around stars.
*/

void klmove(int fl)
{
    int n;
    register struct kling *k;
    FLOAT dx, dy;
    int nextx, nexty;
    register int lookx, looky;
    int motion;
    int fudgex, fudgey;
    int qx, qy;
    FLOAT bigger;
    int i;

#ifdef xTRACE
    if (Trace)
        printf("klmove: fl = %d, Etc.nkling = %d\n", fl, Etc.nkling);
#endif
    for (n = 0; n < Etc.nkling; n++) {
        k = &Etc.klingon[n];
        i = 100;
        if (fl)
            i = 100.0 * k->power / Param.klingpwr;
        if (ranf(i) >= Param.moveprob[2 * Move.newquad + fl])
            continue;

        /* compute distance to move */
        motion = ranf(75) - 25;
        motion *= k->avgdist * Param.movefac[2 * Move.newquad + fl];

        /* compute direction */
        dx     = Ship.sectx - k->x + ranf(3) - 1;
        dy     = Ship.secty - k->y + ranf(3) - 1;
        bigger = dx;
        if (dy > bigger)
            bigger = dy;
        if (bigger == 0.0)
            bigger = 1.0;
        dx = dx / bigger + 0.5;
        dy = dy / bigger + 0.5;
        if (motion < 0) {
            motion = -motion;
            dx     = -dx;
            dy     = -dy;
        }
        fudgex = fudgey = 1;
        /* try to move the klingon */
        nextx = k->x;
        nexty = k->y;
        for (; motion > 0; motion--) {
            lookx = nextx + dx;
            looky = nexty + dy;
            if (lookx < 0 || lookx >= NSECTS || looky < 0 || looky >= NSECTS) {
                /* new quadrant */
                qx = Ship.quadx;
                qy = Ship.quady;
                if (lookx < 0)
                    qx -= 1;
                else if (lookx >= NSECTS)
                    qx += 1;
                if (looky < 0)
                    qy -= 1;
                else if (looky >= NSECTS)
                    qy += 1;
                if (qx < 0 || qx >= NQUADS || qy < 0 || qy >= NQUADS || Quad[qx][qy].stars < 0 ||
                    Quad[qx][qy].klings > MAXKLQUAD - 1)
                    break;
                if (!damaged(SRSCAN)) {
                    printf("Клинг из %d,%d убежал в квадрант %d,%d\n", k->x, k->y, qx, qy);
                    motion = Quad[qx][qy].scanned;
                    if (motion >= 0 && motion < 1000)
                        Quad[qx][qy].scanned += 100;
                    motion = Quad[Ship.quadx][Ship.quady].scanned;
                    if (motion >= 0 && motion < 1000)
                        Quad[Ship.quadx][Ship.quady].scanned -= 100;
                }
                Sect[k->x][k->y] = EMPTY;
                Quad[qx][qy].klings += 1;
                Etc.nkling -= 1;
                bmove((char *)&Etc.klingon[Etc.nkling], (char *)k, sizeof *k);
                Quad[Ship.quadx][Ship.quady].klings -= 1;
                k = 0;
                break;
            }
            if (Sect[lookx][looky] != EMPTY) {
                lookx = nextx + fudgex;
                if (lookx < 0 || lookx >= NSECTS)
                    lookx = nextx + dx;
                if (Sect[lookx][looky] != EMPTY) {
                    fudgex = -fudgex;
                    looky  = nexty + fudgey;
                    if (looky < 0 || looky >= NSECTS || Sect[lookx][looky] != EMPTY) {
                        fudgey = -fudgey;
                        break;
                    }
                }
            }
            nextx = lookx;
            nexty = looky;
        }
        if (k && (k->x != nextx || k->y != nexty)) {
            if (!damaged(SRSCAN))
                printf("Клинг из %d,%d переместился в %d,%d\n", k->x, k->y, nextx, nexty);
            Sect[k->x][k->y]                 = EMPTY;
            Sect[k->x = nextx][k->y = nexty] = KLINGON;
        }
    }
    compkldist(0);
}
