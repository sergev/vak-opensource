#include "getpar.h"
#include "trek.h"

char *mrok(int n)
{
    char *s;
    s = "ов";
    if (10 > (n % 100) || (n % 100) > 20)
        switch (n % 10) {
        case 1:
            s = "";
            break;
        case 2:
        case 3:
        case 4:
            s = "а";
        }
    return (s);
}

/*
**  PRINT OUT THE CURRENT SCORE
*/

long score()
{
    register int u;
    register int t;
    long s;
    FLOAT r;
    extern struct cvntab Skitab[];
    static char *vrk1[] = { "ых", "ую", "ые", "ые", "ые", "ых", "ых", "ых", "ых", "ых" };
    static char *vrk2[] = { "", "у", "ы", "ы", "ы", "", "", "", "", "" };
    static char *vrk3[] = { "ых", "ая", "ые", "ые", "ые", "ых", "ых", "ых", "ых", "ых" };
    static char *vrk4[] = { "", "а", "ы", "ы", "ы", "", "", "", "", "" };

    printf("\n*** Ваши результаты:\n");
    s = t = Param.klingpwr / 4 * (u = Game.killk);
    if (t != 0)
        printf("%d клинг%s убит%s   \t\t\t%6d\n", u, mrok(u), u == 1 ? "" : "о", t);
    r = Now.date - Param.date;
    if (r < 1.0)
        r = 1.0;
    r = Game.killk / r;
    s += (t = (int)(400 * r));
    if (t != 0)
        printf("со скоростью %.2f клинга/день  \t\t%6d\n", r, t);
    r = Now.klings;
    r /= (FLOAT)(Game.killk + 1);
    s += (t = (int)(-400 * r));
    if (t != 0)
        printf("%d клинг%s остал%s \t\t\t%6d\n", Now.klings, mrok(Now.klings),
               Now.klings == 1 ? "ся" : "ось", t);
    if (Move.endgame > 0) {
        s += (t = 100 * (u = Game.skill));
        printf("приз за выигранную игру на '%s%s'\t\t%6d\n", Skitab[u + u - 1].abrev,
               Skitab[u + u - 1].full, t);
    }
    if (Game.killed) {
        s -= 500;
        printf("за самоубийство\t\t\t\t  -500\n");
    }
    s += (t = -100 * (u = Game.killb));
    if (t != 0)
        printf("уничтожено %d звездн%s баз%s\t\t%6d\n", u, vrk1[u], vrk2[u], t);
    s += (t = -100 * (u = Game.helps));
    if (t != 0)
        printf("%d просьб%s о помощи\t\t\t%6d\n", u, vrk2[u], t);
    s += (t = -5 * (u = Game.kills));
    if (t != 0)
        printf("%d звезд%s уничтожен%s\t\t\t%6d\n", u, (u < 20 && u > 10) ? "" : vrk2[u % 10],
               u == 1 ? "а" : "о", t);
    s += (t = -150 * (u = Game.killinhab));
    if (t != 0)
        printf("%d обитаем%s звездн%s систем%s уничтожен%s\t%6d\n", u,
               (u < 20 && u > 10) ? "" : vrk3[u % 10], (u < 20 && u > 10) ? "" : vrk3[u % 10],
               (u < 20 && u > 10) ? "" : vrk4[u % 10], u == 1 ? "а" : "о", t);
    if (Ship.ship != ENTERPRISE) {
        s -= 200;
        printf("за разрушение корабля\t\t\t  -200\n");
    }
    s += (t = 3 * (u = Game.captives));
    if (t != 0)
        printf("%d клинг%s капитулировал%s\t\t\t%6d\n", u, mrok(u), u == 1 ? "" : "о", t);
    s += (t = -(u = Game.deaths));
    if (t != 0)
        printf("%d убит%s    \t\t\t\t%6d\n", u, u == 1 ? "  " : "ых", t);
    printf("\n***  ВСЕГО\t\t\t%14ld\n", s);
    return (s);
}
