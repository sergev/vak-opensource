/*
 * Replacement of MS-DOS routines for console input/output.
 */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "conio.h"

static int bgc = 40;

void clrscr()
{
    printf("\033[%dm\033[2J\033[1;1f", bgc);
}

void gotoxy(int x, int y)
{
    printf("\033[%d;%df", y, x);
}

void textcolor(int color)
{
    switch (color % 16) {
    case BLACK:
        printf("\033[0;%d;%dm", 30, bgc);
        break;
    case BLUE:
        printf("\033[0;%d;%dm", 34, bgc);
        break;
    case GREEN:
        printf("\033[0;%d;%dm", 32, bgc);
        break;
    case CYAN:
        printf("\033[0;%d;%dm", 36, bgc);
        break;
    case RED:
        printf("\033[0;%d;%dm", 31, bgc);
        break;
    case MAGENTA:
        printf("\033[0;%d;%dm", 35, bgc);
        break;
    case BROWN:
        printf("\033[0;%d;%dm", 33, bgc);
        break;
    case LIGHTGRAY:
        printf("\033[0;%d;%dm", 37, bgc);
        break;
    case DARKGRAY:
        printf("\033[1;%d;%dm", 30, bgc);
        break;
    case LIGHTBLUE:
        printf("\033[1;%d;%dm", 34, bgc);
        break;
    case LIGHTGREEN:
        printf("\033[1;%d;%dm", 32, bgc);
        break;
    case LIGHTCYAN:
        printf("\033[1;%d;%dm", 36, bgc);
        break;
    case LIGHTRED:
        printf("\033[1;%d;%dm", 31, bgc);
        break;
    case LIGHTMAGENTA:
        printf("\033[1;%d;%dm", 35, bgc);
        break;
    case YELLOW:
        printf("\033[1;%d;%dm", 33, bgc);
        break;
    case WHITE:
        printf("\033[1;%d;%dm", 37, bgc);
        break;
    }
}

static int getch_echo(int echo)
{
    struct termios oldt, newt;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ICANON;

    if (echo)
        newt.c_lflag &= ECHO;
    else
        newt.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

int getch()
{
    return getch_echo(0);
}

int getche()
{
    return getch_echo(1);
}

int ungetch(int ch)
{
    return ungetc(ch, stdin);
}
