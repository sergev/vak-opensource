/*
 * Curses compatibility definitions.
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
 * $Id: curses.h,v 1.3 2006/04/19 17:26:54 ly Exp $
 */

#ifndef WINDOW

#define __CROX_CURSES__ /* Magic sign */
#include <stdio.h>

#define TRUE  1
#define FALSE 0

#define A_REVERSE   0x1
#define A_UNDERLINE 0x2
#define A_BOLD      0x4
#define A_DIM       0x8

#define WINDOW     void
#define WINDOW_BOX void

#define KEY_DOWN (0400 | 'd')
/* The four arrow keys ... */
#define KEY_UP    (0400 | 'u')
#define KEY_LEFT  (0400 | 'l')
#define KEY_RIGHT (0400 | 'r')
/* ... */
#define KEY_NPAGE (0400 | 'n')
/* Next page */
#define KEY_PPAGE (0400 | 'p')
/* Previous page */
#define KEY_HOME (0400 | 'h')
/* Home key (upward+left arrow) */
#define KEY_END (0400 | 'e')
/* end key */
#define KEY_IC (037 & 'T')
/* Insert char or enter insert mode */
#define KEY_DC (0400 | 'x')
/* Delete character */
#define KEY_F0   KEY_F(10) /* Function keys */
#define KEY_F(n) ((0400 | 'A') + (n) - 1)

#define getch curses_getch
/* to avoid name conflict with msdos getch() */

extern int LINES, COLS;

int _gety(void), _getx(void);

WINDOW *initscr(void);
int move(int y, int x);
int addch(int c);
int addstr(char *str);
int attrset(int a);
int attroff(int a);
int refresh(void);
int wrefresh(WINDOW *win);
int endwin(void);
int getch(void);
int getstr(char *str);
int clrtobot(void);
int deleteln(void);
int erase(void);
int inch(void);
int insertln(void);
int cbreak(void);
int beep(void);
WINDOW_BOX *getbox(int by, int bx, int bny, int bnx);
int drawframe(int y, int x, int ny, int nx);
int eraseframe(int y, int x, int ny, int nx);
int putbox(WINDOW_BOX *box);
int deletebox(WINDOW_BOX *box);
void clearok(WINDOW *win, int flag);
int printw(char *fmt, ...);
int mvprintw(int y, int x, char *fmt, ...);

#define curscr          ((WINDOW *)0)
#define stdscr          ((WINDOW *)0)
#define standout()      attrset(A_REVERSE)
#define standend()      attroff(A_REVERSE)
#define echo()          0 /* void */
#define noecho()        0 /* void */
#define nocbreak()      0 /* void */
#define nl()            0 /* void */
#define nonl()          0 /* void */
#define raw()           0 /* void */
#define noraw()         0 /* void */
#define setterm(name)   0 /* void */
#define scrollok(w, on) 0 /* void */
#define leaveok(w, on)  0 /* void */
#define keypad(w, on)   0 /* void */
#define ttytype         "tty"
#define getyx(w, y, x)  (y = _gety(), x = _getx())
/* w should be stdscr */

#define mvaddch(y, x, c)    (move(y, x), addch(c))
#define mvaddstr(y, x, str) (move(y, x), addstr(str))
#define mvgetch(y, x)       (move(y, x), getch())
#define mvgetstr(y, x, str) (move(y, x), getstr(str))
#define mvinch(y, x)        (move(y, x), inch())

#endif
