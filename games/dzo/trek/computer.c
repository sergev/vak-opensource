#include <stdlib.h>

#include "getpar.h"
#include "trek.h"

/*
**  On-Board Computer
**
**      A computer request is fetched from the captain.  The requests
**      are:
**
**      chart -- print a star chart of the known galaxy.  This includes
**              every quadrant that has ever had a long range or
**              a short range scan done of it, plus the location of
**              all starbases.  This is of course updated by any sub-
**              space radio broadcasts (unless the radio is out).
**              The format is the same as that of a long range scan
**              except that ".1." indicates that a starbase exists
**              but we know nothing else.
**
**      trajectory -- gives the course and distance to every know
**              Klingon in the quadrant.  Obviously this fails if the
**              short range scanners are out.
**
**      course -- gives a course computation from whereever you are
**              to any specified location.  If the course begins
**              with a slash, the current quadrant is taken.
**              Otherwise the input is quadrant and sector coordi-
**              nates of the target sector.
**
**      move -- identical to course, except that the move is performed.
**
**      score -- prints out the current score.
**
**      pheff -- "PHaser EFFectiveness" at a given distance.  Tells
**              you how much stuff you need to make it work.
**
**      warpcost -- Gives you the cost in time and units to move for
**              a given distance under a given warp speed.
**
**      impcost -- Same for the impulse engines.
**
**      distresslist -- Gives a list of the currently known starsystems
**              or starbases which are distressed, together with their
**              quadrant coordinates.
**
**      If a command is terminated with a semicolon, you remain in
**      the computer; otherwise, you escape immediately to the main
**      command processor.
*/

struct cvntab Cputab[] = { { "ch", "art", 1, 0, 0 },
                           { "ка", "рта", 1, 0, 0 },
                           { "t", "rajectory", 2, 0, 0 },
                           { "т", "раектория", 2, 0, 0 },
                           { "c", "ourse", 3, 0, 0 },
                           { "ку", "рс", 3, 0, 0 },
                           { "m", "ove", 3, 1, 0 },
                           { "п", "ереместиться", 3, 1, 0 },
                           { "s", "core", 4, 0, 0 },
                           { "сч", "ет", 4, 0, 0 },
                           { "p", "heff", 5, 0, 0 },
                           { "э", "ффаз", 5, 0, 0 },
                           { "w", "arpcost", 6, 0, 0 },
                           { "рд", "виг", 6, 0, 0 },
                           { "i", "mpcost", 7, 0, 0 },
                           { "ри", "мп", 7, 0, 0 },
                           { "d", "istresslist", 8, 0, 0 },
                           { "у", "грозы", 8, 0, 0 },
                           { 0, 0, 0, 0, 0 } };

void prkalc(int course, FLOAT dist)
{
    printf(": курс %d  расст. %.3f\n", course, dist);
}

void computer(int _)
{
    int ix, iy;
    register int i, j;
    int tqx, tqy;
    struct cvntab *r;
    int cost;
    int course;
    FLOAT dist, time;
    FLOAT warpfact;
    struct quad *q;
    register struct event *e;

    if (check_out(COMPUTER))
        return;
    while (1) {
        r = getcodpar("\nЗапрос", Cputab);
        switch (r->value) {
        case 1: /* star chart */
            printf("Компьютерная карта галактики\n\n");
            printf("  ");
            /* print top header */
            for (i = 0; i < NQUADS; i++)
                printf("-%d- ", i);
            printf("\n");
            for (i = 0; i < NQUADS; i++) {
                printf("%d ", i);
                for (j = 0; j < NQUADS; j++) {
                    if (i == Ship.quadx && j == Ship.quady) {
                        printf("$$$ ");
                        continue;
                    }
                    q = &Quad[i][j];
                    /* 1000 or 1001 is special case */
                    if (q->scanned >= 1000)
                        if (q->scanned > 1000)
                            printf(".1. ");
                        else
                            printf("/// ");
                    else if (q->scanned < 0)
                        printf("... ");
                    else
                        printf("%3d ", q->scanned);
                }
                printf("%d\n", i);
            }
            printf("  ");
            /* print bottom footer */
            for (i = 0; i < NQUADS; i++)
                printf("-%d- ", i);
            printf("\n");
            break;

        case 2: /* trajectory */
            if (check_out(SRSCAN)) {
                break;
            }
            if (Etc.nkling <= 0) {
                printf("В этом квадранте нет клингов\n");
                break;
            }
            /* for each Klingon, give the course & distance */
            for (i = 0; i < Etc.nkling; i++) {
                printf("Клинг в %d,%d", Etc.klingon[i].x, Etc.klingon[i].y);
                course = kalc(Ship.quadx, Ship.quady, Etc.klingon[i].x, Etc.klingon[i].y, &dist);
                prkalc(course, dist);
            }
            break;

        case 3: /* course calculation */
            if (readdelim('/')) {
                tqx = Ship.quadx;
                tqy = Ship.quady;
            } else {
                ix = getintpar("Квадрант");
                if (ix < 0 || ix >= NSECTS)
                    break;
                iy = getintpar("К-y");
                if (iy < 0 || iy >= NSECTS)
                    break;
                tqx = ix;
                tqy = iy;
            }
            ix = getintpar("Сектор");
            if (ix < 0 || ix >= NSECTS)
                break;
            iy = getintpar("С-y");
            if (iy < 0 || iy >= NSECTS)
                break;
            course = kalc(tqx, tqy, ix, iy, &dist);
            if (r->value2) {
                warp(-1, course, dist);
                break;
            }
            printf("%d,%d/%d,%d к %d,%d/%d,%d", Ship.quadx, Ship.quady, Ship.sectx, Ship.secty, tqx,
                   tqy, ix, iy);
            prkalc(course, dist);
            break;

        case 4: /* score */
            score();
            break;

        case 5: /* phaser effectiveness */
            dist = getfltpar("Диапазон");
            if (dist < 0.0)
                break;
            dist *= 10.0;
            cost = pow(0.90, dist) * 98.0 + 0.5;
            printf("Эффективность фазеров в этом диапазоне %d%%\n", cost);
            break;

        case 6: /* warp cost (time/energy) */
            dist = getfltpar("Расстояние");
            if (dist < 0.0)
                break;
            warpfact = getfltpar("Скорость");
            if (warpfact <= 0.0)
                warpfact = Ship.warp;
            cost = (dist + 0.05) * warpfact * warpfact * warpfact;
            time = Param.warptime * dist / (warpfact * warpfact);
            printf(
                "Скорость %.2f  расстояние %.2f\nвремя %.2f  энергия %d (%d;защита вкл.) единиц\n",
                warpfact, dist, time, cost, cost + cost);
            break;

        case 7: /* impulse cost */
            dist = getfltpar("Расстояние");
            if (dist < 0.0)
                break;
            cost = 20 + 100 * dist;
            time = dist / 0.095;
            printf("Расстояние %.2f  энергия %.2f  время %d  единиц\n", dist, time, cost);
            break;

        case 8: /* distresslist */
            j = 1;
            printf("\n");
            /* scan the event list */
            for (i = 0; i < MAXEVENTS; i++) {
                e = &Event[i];
                /* ignore hidden entries */
                if (e->evcode & E_HIDDEN)
                    continue;
                switch (e->evcode & E_EVENT) {
                case E_KDESB:
                    printf("Клинг атаковал звездную базу в квадранте %d,%d\n", e->x, e->y);
                    j = 0;
                    break;

                case E_ENSLV:
                case E_REPRO:
                    printf("Атакована звездная система %s в квадранте %d,%d\n",
                           Systemname[e->systemname], e->x, e->y);
                    j = 0;
                    break;
                }
            }
            if (j)
                printf("Не было сообщений о различных бедствиях\n");
            break;
        }

        /* skip to next semicolon or newline.  Semicolon
         * means get new computer request; newline means
         * exit computer mode. */
        while ((i = cgetc()) != ';') {
            if (i == '\0')
                exit(1);
            if (i == '\n') {
                ungetc(i, stdin);
                return;
            }
        }
    }
}

/*
**  Course Calculation
**
**      Computes and outputs the course and distance from position
**      sqx,sqy/ssx,ssy to tqx,tqy/tsx,tsy.
*/

int kalc(int tqx, int tqy, int tsx, int tsy, FLOAT *dist)
{
    FLOAT dx, dy;
    FLOAT quadsize;
    FLOAT angle;
    register int course;

    /* normalize to quadrant distances */
    quadsize = NSECTS;
    dx       = (Ship.quadx + Ship.sectx / quadsize) - (tqx + tsx / quadsize);
    dy       = (tqy + tsy / quadsize) - (Ship.quady + Ship.secty / quadsize);

    /* get the angle */
    angle = atan2(dy, dx);
    /* make it 0 -> 2 pi */
    if (angle < 0.0)
        angle += 6.283185307;
    /* convert from radians to degrees */
    course = angle * 57.29577951 + 0.5;
    dx     = dx * dx + dy * dy;
    *dist  = sqrt(dx);
    return (course);
}
