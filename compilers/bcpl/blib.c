/* Copyright (c) 2004 Robert Nordier.  All rights reserved. */

#include <stdio.h>
#include <stdlib.h>
#include "blib.h"

#define FTSZ 20

extern int *M;

static FILE *ft[FTSZ];
static int fi, fo;

int
getbyte(s, i)
    int s, i;
{
    int w = M[s + i / 4];
    int m = (i % 4) ^ 3;
    w = w >> (8 * m);
    return w & 255;
}

void
putbyte(s, i, ch)
{
    int p = s + i / 4;
    int m = (i % 4) ^ 3;
    int w = M[p];
    int x = 0xff;
    x = x << (8 * m);
    x = x ^ 0xffffffff;
    w = w & x;
    x = ch;
    x = x & 0xff;
    x = x << (8 * m);
    w = w | x;
    M[p] = w;
}

static char *
cstr(s)
{
    char *st;
    int n, i;

    n = getbyte(s, 0);
    st = malloc(n + 1);
    for (i = 1; i <= n; i++)
        st[i - 1] = getbyte(s, i);
    st[n] = 0;
    return st;
}

static int
ftslot()
{
    int i;

    for (i = 3; i < FTSZ; i++)
        if (ft[i] == NULL)
            return i;
    return -1;
}

void
initio()
{
    ft[0] = stdin;
    ft[1] = stdout;
    ft[2] = stderr;
    fi = 0;
    fo = 1;
}

int
findinput(s)
    int s;
{
    char *st = cstr(s);
    int x;

    x = ftslot();
    if (x != -1) {
        ft[x] = fopen(st, "r");
        if (ft[x] == NULL)
            x = -1;
    }
    free(st);
    return x + 1;
}

int
findoutput(s)
    int s;
{
    char *st = cstr(s);
    int x;

    x = ftslot();
    if (x != -1) {
        ft[x] = fopen(st, "w");
        if (ft[x] == NULL)
            x = -1;
    }
    free(st);
    return x + 1;
}

void
selectinput(x)
    int x;
{
    fi = x - 1;
}

void
selectoutput(x)
    int x;
{
    fo = x - 1;
}

int
input()
{
    return fi + 1;
}

int
output()
{
    return fo + 1;
}

int
rdch()
{
    return fgetc(ft[fi]);
}

void
wrch(c)
    int c;
{
    fputc(c, ft[fo]);
}

void
endread()
{
    if (fi > 2) {
        fclose(ft[fi]);
        ft[fi] = NULL;
    }
    fi = 0;
}

void
endwrite()
{
    if (fo > 2) {
        fclose(ft[fo]);
        ft[fo] = NULL;
    } else
        fflush(ft[fo]);
    fo = 1;
}
