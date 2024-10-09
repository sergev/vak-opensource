#include "getpar.h"
#include "trek.h"

/**
 **     move under impulse power
 **/

void impulse(int _)
{
    int course;
    register int power;
    FLOAT dist, time;
    register int percent;

    if (Ship.cond == DOCKED) {
        printf("Скотти: Сожалею, капитан, но мы стыкованы с базой.\n");
        return;
    }
    if (damaged(IMPULSE)) {
        out(IMPULSE);
        return;
    }
    if (getcodi(&course, &dist))
        return;
    power   = 20 + 100 * dist;
    percent = 100 * power / Ship.energy + 0.5;
    if (percent >= 85) {
        printf("Скотти: На это уйдет %d%% оставшейся энергии.\n", percent);
        if (!getynpar("Делать это"))
            return;
        printf("Ох, сэр\n");
    }
    time    = dist / 0.095;
    percent = 100 * time / Now.time + 0.5;
    if (percent >= 85) {
        printf("Спок: На это нужно %d%% оставшегося времени.\n", percent);
        if (!getynpar("Делать это"))
            return;
        printf("(Он окончательно сошел с ума)\n");
    }
    Move.time = move(0, course, time, 0.095);
    Ship.energy -= 20 + 100 * Move.time * 0.095;
}
