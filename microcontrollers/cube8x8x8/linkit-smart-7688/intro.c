#include "cube.h"

void effect_intro(int delay)
{
    int cnt, cnt_2, time;

    /* Bottom To Top */
    for (cnt=0; cnt<=7; cnt++) {
        box_wireframe(0, 0, 0, 7, 7, cnt);
        delay_ms(delay);
    }
    for (cnt=0; cnt<7; cnt++) {
        clrplane(AXIS_Z, cnt);
        delay_ms(delay);
    }

    /* Shift Things Right */
    /* 1 */
    shift(AXIS_Y, -1);
    for (cnt=0; cnt<=7; cnt++) {
        setvoxel(cnt, 0, 6);
    }
    delay_ms(delay);

    /* 2 */
    shift(AXIS_Y, -1);
    for (cnt=0; cnt<=7; cnt++) {
        setvoxel(cnt, 0, 5);
    }
    setvoxel(0, 0, 6);
    setvoxel(7, 0, 6);
    delay_ms(delay);

    /* 3 */
    shift(AXIS_Y, -1);
    for (cnt=0; cnt<=7; cnt++) {
        setvoxel(cnt, 0, 4);
    }
    setvoxel(0, 0, 5);
    setvoxel(7, 0, 5);
    setvoxel(0, 0, 6);
    setvoxel(7, 0, 6);
    delay_ms(delay);

    /* 4 */
    shift(AXIS_Y, -1);
    for (cnt=0; cnt<=7; cnt++) {
        setvoxel(cnt, 0, 3);
    }
    setvoxel(0, 0 ,4);
    setvoxel(7, 0, 4);
    setvoxel(0, 0, 5);
    setvoxel(7, 0, 5);
    setvoxel(0, 0, 6);
    setvoxel(7, 0, 6);
    delay_ms(delay);

    /* 5 */
    shift(AXIS_Y, -1);
    for (cnt=0; cnt<=7; cnt++) {
        setvoxel(cnt, 0, 2);
    }
    setvoxel(0, 0, 3);
    setvoxel(7, 0, 3);
    setvoxel(0, 0, 4);
    setvoxel(7, 0, 4);
    setvoxel(0, 0, 5);
    setvoxel(7, 0, 5);
    setvoxel(0, 0, 6);
    setvoxel(7, 0, 6);
    delay_ms(delay);

    /* 6 */
    shift(AXIS_Y, -1);
    for (cnt=0; cnt<=7; cnt++) {
        setvoxel(cnt, 0, 1);
    }
    setvoxel(0, 0, 2);
    setvoxel(7, 0, 2);
    setvoxel(0, 0, 3);
    setvoxel(7, 0, 3);
    setvoxel(0, 0, 4);
    setvoxel(7, 0, 4);
    setvoxel(0, 0, 5);
    setvoxel(7, 0, 5);
    delay_ms(delay);

    /* 7 */
    shift(AXIS_Y, -1);
    for (cnt=0; cnt<=7; cnt++) {
        setvoxel(cnt, 0, 0);
    }
    setvoxel(0, 0, 1);
    setvoxel(7, 0, 1);
    setvoxel(0, 0, 2);
    setvoxel(7, 0, 2);
    setvoxel(0, 0, 3);
    setvoxel(7, 0, 3);
    setvoxel(0, 0, 4);
    setvoxel(7, 0, 4);
    setvoxel(0, 0, 5);
    setvoxel(7, 0, 5);
    delay_ms(delay);

    /* Right To Left */
    for (cnt=0; cnt<=7; cnt++) {
        box_wireframe(0, 0, 0, 7, cnt, 7);
        delay_ms(delay);
    }
    for (cnt=0; cnt<7; cnt++) {
        clrplane(AXIS_Y, cnt);
        delay_ms(delay);
    }

    /* Shift to the bottom */
    for (cnt_2=6; cnt_2>=0; cnt_2--) {
        shift(AXIS_Z, -1);
        for (cnt=0; cnt<=7; cnt++) {
            setvoxel(cnt, cnt_2, 0);
        }
        for (cnt=6; cnt>cnt_2; cnt--) {
            setvoxel(0, cnt, 0);
            setvoxel(7, cnt, 0);
        }
        delay_ms(delay);
    }

    /* Make All Wall Box */
    for (cnt=0; cnt<=6; cnt++) {
        fill(0);
        box_walls(0, 0, 0, 7, 7, cnt);
        delay_ms(delay);
    }

    time = delay;
    for (cnt_2=0; cnt_2<5; cnt_2++) {
        time -= delay*3/20;

        /* Make Box Smaller */
        for (cnt=0; cnt<=3; cnt++) {
            fill(0);
            box_walls(cnt, cnt, cnt, 7-cnt, 7-cnt, 7-cnt);
            delay_ms(time);
        }

        /* Make Box Bigger */
        for (cnt=0; cnt<=3; cnt++) {
            fill(0);
            box_walls(3-cnt, 3-cnt, 3-cnt, 4+cnt, 4+cnt, 4+cnt);
            delay_ms(time);
        }
    }
    for (cnt_2=0; cnt_2<5; cnt_2++) {
        time += delay*3/20;

        /* Make Box Smaller */
        for (cnt=0; cnt<=3; cnt++) {
            fill(0);
            box_walls(cnt, cnt, cnt, 7-cnt, 7-cnt, 7-cnt);
            delay_ms(time);
        }

        /* Make Box Bigger */
        for (cnt=0; cnt<=3; cnt++) {
            fill(0);
            box_walls(3-cnt, 3-cnt, 3-cnt, 4+cnt, 4+cnt, 4+cnt);
            delay_ms(time);
        }
    }
    delay_ms(500);
}
