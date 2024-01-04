/*
  VTL-C
  System-dependent module for MinGW

  T. Nakagawa

  2004/05/23
*/

#include "system.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "conio.h"

#define IMGFILE "vtl.img"
#define AUTOSAVELOAD

unsigned char Lct[MEMSIZE];

static void loadimg(void);
static void saveimg(void);

void initl(void)
{
    WRITEW(Lmt, MEMSIZE); /* RAM End */
    WRITEW(Bnd, Obj);     /* Program End */

#ifdef AUTOSAVELOAD
    loadimg();
    atexit(saveimg);
#endif
    signal(SIGINT, exit);
    signal(SIGTERM, exit);

    return;
}

unsigned char getchr(void)
{
    int c;
    c = getch();
    if (c < 0)
        c = 0;
    putch(c);
    return (unsigned char)c;
}

void putchr(unsigned char c)
{
    putch(c);
    return;
}

#ifdef AUTOSAVELOAD
static void loadimg(void)
{
    int i;
    unsigned short p;
    FILE *fp;

    if ((fp = fopen(IMGFILE, "rt")) != NULL) {
        /* Variable Area */
        for (i = 0; i < 0x40; i++) {
            int val;
            if (fscanf(fp, "%*2c%i", &val) == EOF)
                break;
            WRITEW((2 + i) * 2, (unsigned short)val);
        }
        /* Program Area */
        for (p = Obj; p + 1 < READW(Lmt);) {
            int n;
            if (fscanf(fp, "%5d", &n) == EOF)
                break;
            WRITEW(p, (unsigned short)n);
            p += 2;
            for (; p < READW(Lmt);) {
                int c;
                if ((c = getc(fp)) == EOF)
                    break;
                if (c == '\n') {
                    WRITEB(p, 0x00);
                    p++;
                    break;
                } else {
                    WRITEB(p, (unsigned char)c);
                    p++;
                }
            }
        }
        WRITEW(Bnd, p); /* Program End */
        fclose(fp);
    }

    return;
}
#endif

#ifdef AUTOSAVELOAD
static void saveimg(void)
{
    int i;
    unsigned short p;
    FILE *fp;

    if ((fp = fopen(IMGFILE, "wt")) != NULL) {
        /* Variable Area */
        for (i = 0; i < 0x40; i++) {
            fprintf(fp, "%c 0x%04x\n", ((i + 0x20) & 0x3f) + 0x20, READW((2 + i) * 2));
        }
        /* Program Area */
        for (p = Obj; p + 1 < READW(Bnd);) {
            fprintf(fp, "%.5d", READW(p));
            p += 2;
            for (; p < READW(Bnd);) {
                if (READB(p) == 0x00) {
                    putc('\n', fp);
                    p++;
                    break;
                } else {
                    putc(READB(p), fp);
                    p++;
                }
            }
        }
        fclose(fp);
    }

    return;
}
#endif
