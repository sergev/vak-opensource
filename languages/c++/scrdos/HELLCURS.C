/*
 * Simple example of curses based program.
 *
 * Copyright (C) 1994 Cronyx Ltd.
 * Author: Serge Vakulenko, <vak@zebub.msk.su>
 *
 * This software is distributed with NO WARRANTIES, not even the implied
 * warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Authors grant any other persons or organisations permission to use
 * or modify this software as long as this message is kept with the software,
 * all derivative works or modified versions.
 *
 * $Id: hellcurs.c,v 1.2 2006/04/19 17:26:54 ly Exp $
 */
#include "curses.h"

int main()
{
    initscr();
    mvaddstr(LINES / 2 - 3, COLS / 2 - 9, "Hello, World!");
    attrset(A_REVERSE);
    mvaddstr(LINES / 2 - 2, COLS / 2 - 8, "Hello, World!");
    attrset(A_BOLD);
    mvaddstr(LINES / 2 - 1, COLS / 2 - 7, "Hello, World!");
    attrset(A_REVERSE | A_BOLD);
    mvaddstr(LINES / 2 + 0, COLS / 2 - 6, "Hello, World!");
    attrset(A_DIM);
    mvaddstr(LINES / 2 + 1, COLS / 2 - 5, "Hello, World!");
    attrset(A_REVERSE | A_DIM);
    mvaddstr(LINES / 2 + 2, COLS / 2 - 4, "Hello, World!");
    attrset(0);
    refresh();
    getch();
    move(LINES - 1, 0);
    refresh();
    endwin();
    initscr();
    mvaddstr(LINES / 2 - 3, COLS / 2 - 9, "Hello, World!");
    attrset(A_REVERSE);
    mvaddstr(LINES / 2 - 2, COLS / 2 - 8, "Hello, World!");
    attrset(A_BOLD);
    mvaddstr(LINES / 2 - 1, COLS / 2 - 7, "Hello, World!");
    attrset(A_REVERSE | A_BOLD);
    mvaddstr(LINES / 2 + 0, COLS / 2 - 6, "Hello, World!");
    attrset(A_DIM);
    mvaddstr(LINES / 2 + 1, COLS / 2 - 5, "Hello, World!");
    attrset(A_REVERSE | A_DIM);
    mvaddstr(LINES / 2 + 2, COLS / 2 - 4, "Hello, World!");
    attrset(0);
    refresh();
    getch();
    move(LINES - 1, 0);
    refresh();
    endwin();
    return (0);
}
