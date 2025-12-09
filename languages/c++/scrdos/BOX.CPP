//
// Subwindow (box) implementation.
//
// Copyright (C) 1994 Cronyx Ltd.
// Author: Serge Vakulenko, <vak@zebub.msk.su>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
#include <assert.h>

#include "extern.h"
#include "screen.h"

const char *GetSubstring(const char *strings, int index)
{
    if (index < 0)
        return "";

    while (index--)
        while (*strings++)
            ;
    return strings;
}

const char *GetNumerateSuffix(int Value, const char *Suffixes)
{
    Value %= 100;
    if (Value != 11 && (Value + 8) % 10 == 9)
        return Suffixes;
    if ((Value < 10 || Value > 20) && (Value + 8) % 10 < 3)
        return GetSubstring(Suffixes, 1);
    return GetSubstring(Suffixes, 2);
}

Box::Box(ScreenImp &scr, int by, int bx, int bny, int bnx)
{
    y  = by;
    x  = bx;
    ny = bny;
    nx = bnx;

    // if(by < 0 || bx < 0)
    //    printf("\n\rbx = %d, by = %d, bny = %d, bnx = %d\n\r", by, bx, bny,
    //    bnx);

    assert(by >= 0 && bx >= 0);

    mem = new short[ny * nx];

    if (!mem)
        return;
    for (int yy = 0; yy < ny; ++yy) {
        short *p = &scr.scr[yy + y][x];
        short *q = &mem[yy * nx];

        for (int xx = 0; xx < nx; ++xx)
            *q++ = *p++;
    }
}

void ScreenImp::Put(Box &box, int attr)
{
    if (!box.mem)
        return;
    if (attr)
        attr = attr << 8 & 0x7f00;
    for (int yy = 0; yy < box.ny; ++yy) {
        short *q = &box.mem[yy * box.nx];

        for (int xx = 0; xx < box.nx; ++xx)
            pokeChar(box.y + yy, box.x + xx,
                     attr ? (*q++ & 0377) | attr : *q++);
    }
}
