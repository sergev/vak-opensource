/*
 * Replacement of MS-DOS routines for console input/output.
 */
void clrscr(void);
void gotoxy(int x, int y);
void textcolor(int color);
int getch(void);
int getche(void);
int ungetch(int ch);

#define cprintf printf

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15
#define BLINK 128
