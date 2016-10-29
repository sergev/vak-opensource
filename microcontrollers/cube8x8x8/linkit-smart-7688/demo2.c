/*
 * Imported from https://github.com/tomazas/ledcube8x8x8/blob/master/firmware/888.c
 */
#include <stdio.h>
#include "cube.h"

/* rank: A, 1, 2, 3, 4, I,Ä, U */

static const unsigned char table_cha[8][8]= {
    { 0x51, 0x51, 0x51, 0x4a, 0x4a, 0x4a, 0x44, 0x44 },
    { 0x18, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c },
    { 0x3c, 0x66, 0x66, 0x30, 0x18, 0xc,  0x6,  0xf6 },
    { 0x3c, 0x66, 0x60, 0x38, 0x60, 0x60, 0x66, 0x3c },
    { 0x30, 0x38, 0x3c, 0x3e, 0x36, 0x7e, 0x30, 0x30 },
    { 0x3c, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c },
    { 0x66, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x18 },
    { 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x3c }
};

/* the "ideasoft" */

static const unsigned char table_id[40]= {
    0x81, 0xff, 0x81, 0x00, 0xff, 0x81, 0x81, 0x7e,
    0x00, 0xff, 0x89, 0x89, 0x00, 0xf8, 0x27, 0x27,
    0xf8, 0x00, 0x8f, 0x89, 0x89, 0xf9, 0x00, 0xff,
    0x81, 0x81, 0xff, 0x00, 0xff, 0x09, 0x09, 0x09,
    0x01, 0x0,  0x01, 0x01, 0xff, 0x01, 0x01, 0x00
};

/* railway */

static const unsigned char dat[128]= {
    0x0,  0x20, 0x40, 0x60, 0x80, 0xa0, 0xc0, 0xe0,
    0xe4, 0xe8, 0xec, 0xf0, 0xf4, 0xf8, 0xfc, 0xdc,
    0xbc, 0x9c, 0x7c, 0x5c, 0x3c, 0x1c, 0x18, 0x14,
    0x10, 0xc,  0x8,  0x4,  0x25, 0x45, 0x65, 0x85,
    0xa5, 0xc5, 0xc9, 0xcd, 0xd1, 0xd5, 0xd9, 0xb9,
    0x99, 0x79, 0x59, 0x39, 0x35, 0x31, 0x2d, 0x29,
    0x4a, 0x6a, 0x8a, 0xaa, 0xae, 0xb2, 0xb6, 0x96,
    0x76, 0x56, 0x52, 0x4e, 0x6f, 0x8f, 0x93, 0x73,
    0x6f, 0x8f, 0x93, 0x73, 0x4a, 0x6a, 0x8a, 0xaa,
    0xae, 0xb2, 0xb6, 0x96, 0x76, 0x56, 0x52, 0x4e,
    0x25, 0x45, 0x65, 0x85, 0xa5, 0xc5, 0xc9, 0xcd,
    0xd1, 0xd5, 0xd9, 0xb9, 0x99, 0x79, 0x59, 0x39,
    0x35, 0x31, 0x2d, 0x29, 0x0,  0x20, 0x40, 0x60,
    0x80, 0xa0, 0xc0, 0xe0, 0xe4, 0xe8, 0xec, 0xf0,
    0xf4, 0xf8, 0xfc, 0xdc, 0xbc, 0x9c, 0x7c, 0x5c,
    0x3c, 0x1c, 0x18, 0x14, 0x10, 0xc,  0x8,  0x4
};

/* railway 3 */

static const unsigned char dat3[24]= {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x16,
    0x26, 0x36, 0x46, 0x56, 0x66, 0x65, 0x64, 0x63,
    0x62, 0x61, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10
};

/* 3p char */

static const unsigned char table_3p[3][8]= {
    { 0xff, 0x89, 0xf5, 0x93, 0x93, 0xf5, 0x89, 0xff },
    { 0x0e, 0x1f, 0x3f, 0x7e, 0x7e, 0x3f, 0x1f, 0x0e },
    { 0x18, 0x3c, 0x7e, 0xff, 0x18, 0x18, 0x18, 0x18 }
};

static void trailler(int delay)
{
    int i, j;

    for (i=6; i>=-3; i--) {
        if (i >= 0) {
            for (j=0; j<8; j++)
                cube[j][i] = cube[j][i+1];
        }
        if (i < 4) {
            for (j=0; j<8; j++)
                cube[j][i+4] = 0;
        }
        delay_ms(delay);
    }
}

static void point(int x, int y, int z, int le)
{
    if (le)
        cube[z][y] |= 1 << x;
    else
        cube[z][y] &= ~(1 << x);
}

static void type(int cha, int y)
{
    int x;

    for (x=0; x<8; x++) {
        cube[x][y] = table_cha[cha][x];
    }
}

/*
 * The first variable is the distance from the midpoint.
 * The second is the layer.
 * The third is the flash speed of the time between each two point.
 * The forth is the enable io, it controls weather draw or clean.
 */
static void cirp(int cpp, int dir, int le)
{
    int a, b, c, cp;

    if (cpp < 128 && cpp >= 0) {
        if (dir)
            cp = 127-cpp;
        else
            cp = cpp;
        a = (dat[cp] >> 5) & 7;
        b = (dat[cp] >> 2) & 7;
        c = dat[cp] & 3;
        if (cpp > 63)
            c = 7-c;
        point(a, b, c, le);
    }
}

/*
 * Reorder two values to make first one smaller than the second one.
 */
static void reorder(int *a, int *b)
{
    int t;

    if (*a > *b) {
        t = *a;
        *a = *b;
        *b = t;
    }
}

static void box_apeak_xy(int x1, int y1, int z1, int x2, int y2, int z2, int fill, int le)
{
    int i;

    reorder(&z1, &z2);
    if (fill) {
        for (i=z1; i<=z2; i++) {
            line(x1, y1, i, x2, y2, i, le);
        }
    } else {
        line(x1, y1, z1, x2, y2, z1, le);
        line(x1, y1, z2, x2, y2, z2, le);
        line(x2, y2, z1, x2, y2, z2, le);
        line(x1, y1, z1, x1, y1, z2, le);
    }
}

static void poke(int n, int x, int y)
{
    int i;

    for (i=0; i<8; i++) {
        point(x, y, i, (n >> i) & 1);
    }
}

static void boxtola(int i, int n)
{
    if (i>=0 && i<8) {
        poke(n, 0, 7-i);
    }
    if (i>=8 && i<16) {
        poke(n, i-8, 0);
    }
    if (i>=16 && i<24) {
        poke(n, 7, i-16);
    }
}

static void rolldisplay(int delay)
{
    int j, i, a;

    for (i=23; i>-40; i--) {
        for (j=0; j<40; j++) {
            a = i+j;
            if (a>=0 && a<24)
                boxtola(a, table_id[j]);
        }
        delay_ms(delay);
    }
}

static void roll_apeak_yz(int n, int delay)
{
    int i;

    switch(n) {
    case 1:
        for (i=0; i<7; i++) {
            cube[i][7] = 0;
            cube[7][6-i] = 255;
            delay_ms(delay);
        };
        break;
    case 2:
        for (i=0; i<7; i++) {
            cube[7][7-i] = 0;
            cube[6-i][0] = 255;
            delay_ms(delay);
        };
        break;
    case 3:
        for (i=0; i<7; i++) {
            cube[7-i][0] = 0;
            cube[0][i+1] = 255;
            delay_ms(delay);
        };
        break;
    case 0:
        for (i=0; i<7; i++) {
            cube[0][i] = 0;
            cube[i+1][7] = 255;
            delay_ms(delay);
        };
    }
}

static void roll_apeak_xy(int n, int delay)
{
    int i;

    switch(n) {
    case 1:
        for (i=0; i<7; i++) {
            line(0, i, 0, 0, i, 7, 0);
            line(i+1, 7, 0, i+1, 7, 7, 1);
            delay_ms(delay);
        };
        break;
    case 2:
        for (i=0; i<7; i++) {
            line(i, 7, 0, i, 7, 7, 0);
            line(7, 6-i, 0, 7, 6-i, 7, 1);
            delay_ms(delay);
        };
        break;
    case 3:
        for (i=0; i<7; i++) {
            line(7, 7-i, 0, 7, 7-i, 7, 0);
            line(6-i, 0, 0, 6-i, 0, 7, 1);
            delay_ms(delay);
        };
        break;
    case 0:
        for (i=0; i<7; i++) {
            line(7-i, 0, 0, 7-i, 0, 7, 0);
            line(0, i+1, 0, 0, i+1, 7, 1);
            delay_ms(delay);
        };
    }
}

static void roll_3_xy(int n, int delay)
{
    int i;

    switch(n) {
    case 1:
        for (i=0; i<8; i++) {
            box_apeak_xy(0, i, 0, 7, 7-i, 7, 1, 1);
            delay_ms(delay);
            if (i<7)
                box_apeak_xy(3, 3, 0, 0, i, 7, 1, 0);
        };
        break;
    case 2:
        for (i=0; i<8; i++) {
            box_apeak_xy(7-i, 0, 0, i, 7, 7, 1, 1);
            delay_ms(delay);
            if (i<7)
                box_apeak_xy(3, 4, 0, i, 7, 7, 1, 0);
        };
        break;
    case 3:
        for (i=0; i<8; i++) {
            box_apeak_xy(0, i, 0, 7, 7-i, 7, 1, 1);
            delay_ms(delay);
            if (i<7)
                box_apeak_xy(4, 4, 0, 7, 7-i, 7, 1, 0);
        };
        break;
    case 0:
        for (i=0; i<8; i++) {
            box_apeak_xy(7-i, 0, 0, i, 7, 7, 1, 1);
            delay_ms(delay);
            if (i<7)
                box_apeak_xy(4, 3, 0, 7-i, 0, 7, 1, 0);
        };
    }
}

static void trans(int z, int delay)
{
    int i, j;

    for (j=0; j<8; j++) {
        for (i=0; i<8; i++) {
            cube[z][i] >>= 1;
        }
        delay_ms(delay);
    }
}

static void tranoutchar(int c, int delay)
{
    int i, j, k, a, i2 = 0;

    for (i=0; i<8; i++) {
        if (i < 7)
            box_apeak_xy(i+1, 0, 0, i+1, 7, 7, 1, 1);
        box_apeak_xy(i2, 0, 0, i2, 7, 7, 1, 0);
        a = 0;
        i2 = i+1;
        for (j=0; j<=i; j++) {
            a |= 1<<j;
        }
        for (k=0; k<8; k++) {
            cube[k][3] |= table_cha[c][k] & a;
            cube[k][4] |= table_cha[c][k] & a;
        }
        delay_ms(delay);
    }
}

/* From now on, the function below is to display the flash. */

void flash_1()
{
    fill(0);
    type(1, 0);
    delay_ms(300);
    type(2, 0);
    delay_ms(300);
    type(3, 0);
    delay_ms(300);
    type(4, 0);
    delay_ms(300);
    delay_ms(300);
    fill(0);
    rolldisplay(150);
    type(0, 7);
    delay_ms(300);
    trailler(30);
    delay_ms(300);
}

void flash_2()
{
    int i;

    for (i=129; i>0; i--) {
        cirp(i-2, 0, 1);
        delay_ms(40);
        cirp(i-1, 0, 0);
    }
    delay_ms(40);
    for (i=0; i<136; i++) {
        cirp(i, 1, 1);
        delay_ms(40);
        cirp(i-8, 1, 0);
    }
    delay_ms(40);
    for (i=129; i>0; i--) {
        cirp(i-2, 0, 1);
        delay_ms(40);
    }
    delay_ms(40);
    for (i=0; i<128; i++) {
        cirp(i-8, 1, 0);
        delay_ms(40);
    }
    delay_ms(300);
}

void flash_3()
{
    int i;

    for (i=0; i<8; i++) {
        box_apeak_xy(0, i, 0, 7, i, 7, 1, 1);
        delay_ms(100);
        if (i<7)
            box_apeak_xy(0, i, 0, 7, i, 7, 1, 0);
    }
    for (i=7; i>=0; i--) {
        box_apeak_xy(0, i, 0, 7, i, 7, 1, 1);
        delay_ms(100);
        if (i>0)
            box_apeak_xy(0, i, 0, 7, i, 7, 1, 0);
    }
    for (i=0; i<8; i++) {
        box_apeak_xy(0, i, 0, 7, i, 7, 1, 1);
        delay_ms(100);
        if (i<7)
            box_apeak_xy(0, i, 0, 7, i, 7, 1, 0);
    }
}

void flash_5()
{
    int delay = 75;
    int i = 8, j, an[4];

    /* 1 */
    for (j=7; j<11; j++)
        an[j-7] = j;
    while (i--) {
        for (j=0; j<4; j++) {
            if (an[j] < 8)
                box_apeak_xy(j, an[j], j, 7-j, an[j], 7-j, 0, 1);
            if (an[j] < 7)
                box_apeak_xy(j, an[j]+1, j, 7-j, an[j]+1, 7-j, 0, 0);
        }
        for (j=0; j<4; j++) {
            if (an[j] > 3)
                an[j]--;
        }
        delay_ms(delay);
    }

    /* 2 */
    i = 3;
    for (j=0; j<4; j++)
        an[j] = 5-j;
    while (i--) {
        for (j=1; j<4; j++) {
            if (an[j] < 4)
                box_apeak_xy(j, an[j], j, 7-j, an[j], 7-j, 0, 1);
            if (an[j] < 3)
                box_apeak_xy(j, an[j]+1, j, 7-j, an[j]+1, 7-j, 0, 0);
        }
        for (j=0; j<4; j++) {
            if (an[j] > 0)
                an[j]--;
        }
        delay_ms(delay);
    }

    /* 3 */
    i = 3;
    for (j=1; j<4; j++)
        an[j] = 4-j;
    while (i--) {
        for (j=1; j<4; j++) {
            if (an[j] >= 0)
                box_apeak_xy(j, an[j], j, 7-j, an[j], 7-j, 0, 1);
            if (an[j] > 0)
                box_apeak_xy(j, an[j]-1, j, 7-j, an[j]-1, 7-j, 0, 0);
        }
        for (j=1; j<4; j++) {
            if (an[j] < 3)
                an[j]++;
        }
        delay_ms(delay);
    }

    /* 4 */
    i = 3;
    for (j=0; j<4; j++)
        an[j] = j+1;
    while (i--) {
        for (j=1; j<4; j++) {
            if (an[j] > 3)
                box_apeak_xy(j, an[j], j, 7-j, an[j], 7-j, 0, 1);
            if (an[j] > 3)
                box_apeak_xy(j, an[j]-1, j, 7-j, an[j]-1, 7-j, 0, 0);
        }
        for (j=0; j<4; j++)
            an[j]++;
        delay_ms(delay);
    }

    /* 5 */
    i = 3;
    for (j=3; j<6; j++)
        an[j-2] = j;
    while (i--) {
        for (j=1; j<4; j++) {
            box_apeak_xy(j, an[j], j, 7-j, an[j], 7-j, 0, 1);
            box_apeak_xy(j, an[j]+1, j, 7-j, an[j]+1, 7-j, 0, 0);
        }
        for (j=0; j<4; j++) {
            if (an[j] > 3)
                an[j]--;
        }
        delay_ms(delay);
    }

    /* 6 */
    i = 3;
    for (j=0; j<4; j++)
        an[j] = 5-j;
    while (i--) {
        for (j=1; j<4; j++) {
            if (an[j] < 4)
                box_apeak_xy(j, an[j], j, 7-j, an[j], 7-j, 0, 1);
            if (an[j] < 3)
                box_apeak_xy(j, an[j]+1, j, 7-j, an[j]+1, 7-j, 0, 0);
        }
        for (j=0; j<4; j++) {
            if (an[j] > 0)
                an[j]--;
        }
        delay_ms(delay);
    }

    /* 7 */
    i = 3;
    for (j=0; j<4; j++)
        an[j] = 3-j;
    an[0] = 2;
    while (i--) {
        for (j=0; j<3; j++) {
            if (an[j] >= 0)
                box_apeak_xy(j, an[j], j, 7-j, an[j], 7-j, 0, 1);
            if (an[j] >= 0)
                box_apeak_xy(j, an[j]+1, j, 7-j, an[j]+1, 7-j, 0, 0);
        }
        for (j=0; j<4; j++) {
            if (j < 5-i)
                an[j]--;
        }
        delay_ms(delay);
    }

    /* 8 */
    i = 10;
    for (j=0; j<4; j++)
        an[j] = j-2;
    while (i--) {
        for (j=0; j<4; j++) {
            if (an[j] >= 0)
                box_apeak_xy(j, an[j], j, 7-j, an[j], 7-j, 0, 1);
            if (an[j] >= 0)
                box_apeak_xy(j, an[j]-1, j, 7-j, an[j]-1, 7-j, 0, 0);
        }
        for (j=0; j<4; j++) {
            if (an[j] < 7)
                an[j]++;
        }
        delay_ms(delay);
    }
}

void flash_6()
{
    int i, j, k, z;

    roll_apeak_yz(1, 50);
    roll_apeak_yz(2, 50);
    roll_apeak_yz(3, 50);
    roll_apeak_yz(0, 50);
    roll_apeak_yz(1, 50);
    roll_apeak_yz(2, 50);
    roll_apeak_yz(3, 50);
    for (i=0; i<3; i++) {
        for (j=0; j<8; j++) {
            for (k=0; k<8; k++) {
                if ((table_3p[i][j] >> k) & 1) {
                    for (z=1; z<8; z++) {
                        point(j, 7-k, z, 1);
                        if (z-1)
                            point(j, 7-k, z-1, 0);
                        delay_ms(25);
                    }
                }
            }
        }
        trans(7, 75);
    }
}

void flash_7()
{
    int i;
    int delay = 15;

    roll_apeak_yz(0, 50);
    roll_apeak_yz(1, 50);
    roll_apeak_yz(2, 50);
    roll_apeak_yz(3, 50);
    roll_apeak_yz(0, 50);
    roll_apeak_yz(1, 50);
    roll_apeak_yz(2, 50);
    roll_apeak_yz(3, 50);
    roll_apeak_yz(0, 50);
    roll_apeak_yz(1, 50);
    roll_apeak_yz(2, 50);
    roll_apeak_xy(0, 50);
    roll_apeak_xy(1, 50);
    roll_apeak_xy(2, 50);
    roll_apeak_xy(3, 50);
    roll_apeak_xy(0, 50);
    roll_apeak_xy(1, 50);
    roll_apeak_xy(2, 50);
    roll_apeak_xy(3, 50);
    for (i=0; i<8; i++) {
        box_apeak_xy(0, i, 0, 7-i, i, 7, 1, 1);
        delay_ms(delay);
    }
    delay_ms(150);
    roll_3_xy(0, delay);
    delay_ms(150);
    roll_3_xy(1, delay);
    delay_ms(150);
    roll_3_xy(2, delay);
    delay_ms(150);
    roll_3_xy(3, delay);
    delay_ms(150);
    roll_3_xy(0, delay);
    delay_ms(150);
    roll_3_xy(1, delay);
    delay_ms(150);
    roll_3_xy(2, delay);
    delay_ms(150);
    roll_3_xy(3, delay);
    for (i=7; i>0; i--) {
        box_apeak_xy(i, 0, 0, i, 7, 7, 1, 0);
        delay_ms(delay);
    }
}

void flash_8()
{
    int i;

    for (i=5; i<8; i++) {
        tranoutchar(i, 50);
        delay_ms(300);
        delay_ms(300);
    }
}

int main()
{
    printf("Cube Demo\n");
    gpio_init();
    gpio_ext(1);

    for (;;) {
        printf("1\n");
        sensor_active = 0;
        fill(0);
        flash_1();

        printf("2\n");
        sensor_active = 0;
        fill(0);
        flash_2();

        printf("3\n");
        sensor_active = 0;
        flash_3();

        printf("5\n");
        sensor_active = 0;
        flash_5();
        flash_5();

        printf("6\n");
        sensor_active = 0;
        flash_6();

        printf("7\n");
        sensor_active = 0;
        flash_7();

        printf("8\n");
        sensor_active = 0;
        flash_8();
    }
}
