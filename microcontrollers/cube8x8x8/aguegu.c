/*
 * aGuegu 2011-08-02 weihong.guan@gmail.com
 * http://aguegu.net/wordpress/wp-content/uploads/2011/11/062-3D8-Controller-Demo.zip
 */
#include <REG51.H>
#include <intrins.h>
#include <stdlib.h>

#define LAYER_COUNT 8
#define COLUMN_COUNT 64

void msleep(int a)
{
    while (a-- > 0)
        usleep(1000);
}

void uart_init()
{
    TMOD = 0x20;
    SCON = 0x50;
    TH1 = 0xff;
    TL1 = 0xff;
    PCON = 0x80;
    TR1 = 1;
}

void uart_send (int data)
{
    SBUF = data;
    while (! TI)
        continue;
    TI = 0;
}

int get_column(int x, int y)
{
    return 8*y + x;
}

void cmd_global(int data)
{
    uart_send(0xf0);
    uart_send(data);
}

void cmd_column(int column, int data)
{
    uart_send(0xf1);
    uart_send(column);
    uart_send(data);
}

void cmd_cube(int *p)
{
    int i;

    uart_send(0xf2);
    for (i=0; i < COLUMN_COUNT; i++)
        uart_send(p[i]);
}

void scan_by_z()
{
    int z;

    cmd_global(0x00);
    for (z=0; z < LAYER_COUNT; z++) {
        cmd_global(0x01 << z);
        msleep(1);
    }
}

void scan_by_x()
{
    int x, y;

    for (x=0; x < LAYER_COUNT; x++) {
        cmd_global(0x00);
        for (y=0; y < LAYER_COUNT; y++)
            cmd_column(get_column(x,y), 0xff);
        msleep(1);
    }
}

void scan_by_y()
{
    int x, y;

    for (y=0; y < LAYER_COUNT; y++) {
        cmd_global(0x00);
        for (x=0; x < LAYER_COUNT; x++)
            cmd_column(get_column(x,y), 0xff);
        msleep(1);
    }
}

void funDemoRise(uint8_t *pCube)
{
    int i, x, y, j;

    j = 0x80;
    while (j--) {

        for (i=0; i < COLUMN_COUNT; i++) {
            pCube[i] <<= 1;
        }

        for (i=0; i < rand() % 4; i++) {
            x = rand() % 8;
            y = rand() % 8;
            pCube[get_column(x,y)] |= 0x01;
        }

        cmd_cube(pCube);
        msleep(1);
    }
}

void main()
{
    int8_t pCube[COLUMN_COUNT];
    int i;

    msleep(4);

    srand(9);

    uart_init();

    for (;;) {
        for (i=0; i<4; i++) {
            cmd_global(0xff);
            msleep(4);
            cmd_global(0x00);
            msleep(4);
        }

        scan_by_x();
        scan_by_y();
        scan_by_z();

        funDemoRise(pCube);
    }
}
