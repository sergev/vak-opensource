//
// Curses compatibility functions.
//
// Copyright (C) 1994-2004 Cronyx Engineering.
// Author: Serge Vakulenko, <vak@zebub.msk.su>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
// $Id: curses.cpp,v 1.6 2008/04/18 12:57:55 ly Exp $
//
#include <stdlib.h>
#include <string.h>
#ifdef unix
#include <unistd.h>
#else
#include <io.h>
#endif
#include "screen.h"

static Screen *V;
static int color, attr, keypad;
static int normalColor, boldColor, dimColor;
static int inverseColor, inverseBoldColor, inverseDimColor;

static void fatal()
{
    const char *msg = "curses not initialized\n";

    write(2, msg, strlen(msg));
    exit(-1);
}

extern "C" {
#include "curses.h"

int COLS, LINES;

int _getx()
{
    if (!V)
        fatal();
    return (V->Col());
}
int _gety()
{
    if (!V)
        fatal();
    return (V->Row());
}

int addch(int c)
{
    if (!V)
        fatal();
    V->Put(c, color);
    return (TRUE);
}

int addstr(char *str)
{
    if (!V)
        fatal();
    V->Put(str, color);
    return (TRUE);
}

int clear()
{
    if (!V)
        fatal();
    V->Clear(color);
    return (TRUE);
}

void clearok(WINDOW *win, int flag)
{
    if (!V)
        fatal();
    V->ClearOk(flag);
}

int refresh()
{
    if (!V)
        fatal();
    V->Sync();
    return (TRUE);
}

int wrefresh(WINDOW *win)
{
    if (!V)
        fatal();
    V->Redraw();
    return (TRUE);
}

int getch()
{
    if (!V)
        fatal();
    return (V->GetKey());
}

int getstr(char *str)
{
    int c;

    if (!V)
        fatal();
    while ((c = V->GetKey()) != '\n')
        *str++ = c;
    *str = 0;
    return (TRUE);
}

int move(int y, int x)
{
    if (!V)
        fatal();
    V->Move(y, x);
    return (TRUE);
}

int endwin()
{
    if (!V)
        fatal();
    if (keypad) {
        V->Close();
        keypad = 0;
    }
    return (TRUE);
}

int cbreak()
{
    if (!V)
        fatal();
    if (!keypad) {
        V->Reopen();
        keypad = 1;
    }
    return (TRUE);
}

WINDOW *initscr()
{
    if (V) {
        V->Reopen();
        return (stdscr);
    }
    V = new Screen();
    if (!V)
        fatal();
    LINES = V->Lines;
    COLS  = V->Columns;
    if (V->HasColors()) {
        normalColor      = 0x1f /*0x70 */;
        boldColor        = 0x1e /*0x71 */;
        dimColor         = 0x1b /*0x75 */;
        inverseColor     = 0x7f /*0x1b */;
        inverseBoldColor = 0x7e /*0x1f */;
        inverseDimColor  = 0x7b /*0x15 */;
    } else {
        normalColor      = 0x07;
        boldColor        = 0x0f;
        dimColor         = 0x07;
        inverseColor     = 0x1f;
        inverseBoldColor = 0x1f;
        inverseDimColor  = 0x10;
    }
    attrset(0);
    clear();
    keypad = 1;
    return (stdscr);
}

int printw(char *fmt, ...)
{
    va_list ap;

    if (!V)
        fatal();
    va_start(ap, fmt);
    V->PrintVect(color, fmt, ap);
    va_end(ap);
    return (TRUE);
}

int mvprintw(int y, int x, char *fmt, ...)
{
    va_list ap;

    if (!V)
        fatal();
    V->Move(y, x);
    va_start(ap, fmt);
    V->PrintVect(color, fmt, ap);
    va_end(ap);

    return (TRUE);
}

int attrset(int a)
{
    if (a & A_UNDERLINE) {
        a |= A_DIM;
        a &= ~A_UNDERLINE;
    }
    if (a & A_BOLD) {
        color = (a & A_REVERSE) ? inverseBoldColor : boldColor;
        a &= ~(A_DIM | A_UNDERLINE);
    } else if (a & (A_DIM | A_UNDERLINE)) {
        color = (a & A_REVERSE) ? inverseDimColor : dimColor;
        a |= A_DIM;
        a &= ~A_UNDERLINE;
    } else
        color = (a & A_REVERSE) ? inverseColor : normalColor;
    attr = a;
    return (TRUE);
}

int attroff(int a)
{
    if (a & (A_BOLD | A_DIM)) {
        attr &= ~(A_BOLD | A_DIM);
        if (a & A_REVERSE) {
            color = normalColor;
            attr &= ~A_REVERSE;
        } else
            color = (attr & A_REVERSE) ? inverseColor : normalColor;
    } else if (a & A_REVERSE) {
        attr &= ~A_REVERSE;
        if (attr & A_BOLD)
            color = boldColor;
        else if (attr & A_DIM)
            color = dimColor;
        else
            color = normalColor;
    }
    return (TRUE);
}

int erase()
{
    if (!V)
        fatal();
    V->Clear(0, 0, V->Lines, V->Columns, color);
    return (TRUE);
}

int beep()
{
    if (!V)
        fatal();
    V->Beep();
    return (TRUE);
}

int deleteln()
{
    if (!V)
        fatal();
    V->DelLine(V->Row(), color);
    return (TRUE);
}

int insertln()
{
    if (!V)
        fatal();
    V->InsLine(V->Row(), color);
    return (TRUE);
}

int inch()
{
    if (!V)
        fatal();
    return (V->GetChar(V->Row(), V->Col()));
}

int clrtoeol()
{
    if (!V)
        fatal();
    V->ClearLine(color);
    return (TRUE);
}

int clrtobot()
{
    if (!V)
        fatal();
    V->ClearLine(color);
    for (int r = V->Row() + 1; r < V->Columns; ++r)
        V->ClearLine(r, 0, color);
    return (TRUE);
}

WINDOW_BOX *getbox(int by, int bx, int bny, int bnx)
{
    if (!V)
        return (0);
    return (new Box(*V, by, bx, bny, bnx));
}

int drawframe(int y, int x, int ny, int nx)
{
    if (!V)
        fatal();
    V->DrawFrame(y, x, ny, nx, color);
    return (TRUE);
}

int eraseframe(int y, int x, int ny, int nx)
{
    if (!V)
        fatal();
    V->Clear(y, x, ny, nx, color);
    return (TRUE);
}

int putbox(WINDOW_BOX *box)
{
    if (!V)
        fatal();
    V->Put(*(Box *)box);
    return (TRUE);
}

int deletebox(WINDOW_BOX *box)
{
    delete (Box *)box;
    return (TRUE);
}

}; /* extern "C" */
