#include <stdio.h>
#include <stdlib.h>

#include "getpar.h"
#include "trek.h"

/*
**  PHOTON TORPEDO CONTROL
**
**      Either one or three photon torpedoes are fired.  If three
**      are fired, it is called a "burst" and you also specify
**      a spread angle.
**
**      Torpedoes are never 100% accurate.  There is always a random
**      cludge factor in their course which is increased if you have
**      your shields up.  Hence, you will find that they are more
**      accurate at close range.  However, they have the advantage that
**      at long range they don't lose any of their power as phasers
**      do, i.e., a hit is a hit is a hit, by any other name.
**
**      When the course spreads too much, you get a misfire, and the
**      course is randomized even more.  You also have the chance that
**      the misfire damages your torpedo tubes.
*/

void torped(int _)
{
    register int ix, iy;
    FLOAT x, y, dx, dy;
    FLOAT angle;
    int course, course2;
    register int k;
    FLOAT bigger;
    FLOAT sectsize;
    int burst;
    int n;

    if (Ship.cloaked) {
        printf("По законам Федерации в закрытом состоянии торпедные атаки запрещены.\n");
        return;
    }
    if (check_out(TORPED))
        return;
    if (Ship.torped <= 0) {
        printf("Все фотонные торпеды израсходованы\n");
        return;
    }

    /* get the course */
    course = getintpar("Курс торпеды");
    if (course < 0 || course > 360)
        return;
    burst = -1;

    /* need at least three torpedoes for a burst */
    if (Ship.torped < 3) {
        printf("Для пактной стрельбы нет торпед\n");
        burst = 0;
    } else {
        /* see if the user wants one */
        if (!testnl()) {
            k = ungetc(cgetc(), stdin);
            if (k >= '0' && k <= '9')
                burst = 1;
        }
    }
    if (burst < 0) {
        burst = getynpar("Стрелять пакетами");
    }
    if (burst) {
        burst = getintpar("Угол рассеяния");
        if (burst <= 0)
            return;
        if (burst > 15) {
            printf("Максимальный угол 15 градусов\n");
            return;
        }
    }
    sectsize = NSECTS;
    n        = -1;
    if (burst) {
        n = 1;
        course -= burst;
    }
    for (; n && n <= 3; n++) {
        /* select a nice random course */
        course2 = course + randcourse(n);
        angle   = course2 * 0.0174532925; /* convert to radians */
        dx      = -cos(angle);
        dy      = sin(angle);
        bigger  = fabs(dx);
        x       = fabs(dy);
        if (x > bigger)
            bigger = x;
        dx /= bigger;
        dy /= bigger;
        x = Ship.sectx + 0.5;
        y = Ship.secty + 0.5;
        if (Ship.cond != DOCKED)
            Ship.torped -= 1;
        printf("Траектория торпеды");
        if (n > 0)
            printf(", торпеда номер %d", n);
        printf(":\n%6.1f\t%4.1f\n", x, y);
        while (1) {
            ix = x += dx;
            iy = y += dy;
            if (x < 0.0 || x >= sectsize || y < 0.0 || y >= sectsize) {
                printf("Промахнулись\n");
                break;
            }
            printf("%6.1f\t%4.1f\n", x, y);
            switch (Sect[ix][iy]) {
            case EMPTY:
                continue;

            case HOLE:
                printf("Торпеда провалилась в черную дыру\n");
                break;

            case KLINGON:
                for (k = 0; k < Etc.nkling; k++) {
                    if (Etc.klingon[k].x != ix || Etc.klingon[k].y != iy)
                        continue;
                    Etc.klingon[k].power -= 500 + ranf(501);
                    if (Etc.klingon[k].power > 0) {
                        printf("*** Удар по клингу в %d,%d: сильно поврежден\n", ix, iy);
                        break;
                    }
                    killk(ix, iy);
                    break;
                }
                break;

            case STAR:
                nova(ix, iy);
                break;

            case INHABIT:
                kills(ix, iy, -1);
                break;

            case BASE:
                killb(Ship.quadx, Ship.quady);
                Game.killb += 1;
                break;
            default:
                printf("Неопознанный обаект %c в %d,%d уничтожен\n", Sect[ix][iy], ix, iy);
                Sect[ix][iy] = EMPTY;
                break;
            }
            break;
        }
        if (damaged(TORPED) || Quad[Ship.quadx][Ship.quady].stars < 0)
            break;
        course += burst;
    }
    Move.free = 0;
}

/*
**  RANDOMIZE COURSE
**
**      This routine randomizes the course for torpedo number 'n'.
**      Other things handled by this routine are misfires, damages
**      to the tubes, etc.
*/

int randcourse(int n)
{
    FLOAT r;
    register int d;

    d = ((franf() + franf()) - 1.0) * 20;
    if (abs(d) > 12) {
        printf("Торпедная установка сломалась");
        if (n < 0)
            printf("\n");
        else
            printf(" на торпеде %d\n", n);
        if (ranf(2)) {
            damage(TORPED, 0.2 * abs(d) * (franf() + 1.0));
        }
        d *= 1.0 + 2.0 * franf();
    }
    if (Ship.shldup || Ship.cond == DOCKED) {
        r = Ship.shield;
        r = 1.0 + r / Param.shield;
        if (Ship.cond == DOCKED)
            r = 2.0;
        d *= r;
    }
    return (d);
}
