// Turbo C++ - (C) Copyright 1987-1991 by Borland International

#ifndef TCALC_H
#define TCALC_H

// Include spreadsheet.h first to get all core definitions
#include "spreadsheet.h"

// File permissions - using POSIX standard
#include <sys/stat.h>

// TUI-specific message strings (spreadsheet.h has MSGNAME, MSGERROR, MSGSTACKERROR)
#define MSGHEADER      "TURBOCALC - A Turbo C++ Demonstration Program"
#define MSGKEYPRESS    "Press any key to continue."
#define MSGCOMMAND     "Press / for the list of commands"
#define MSGMEMORY      "Memory Available:"
#define MSGLOMEM       "Not enough memory to allocate cell."
#define MSGEMPTY       "Empty"
#define MSGTEXT        "Text"
#define MSGVALUE       "Value"
#define MSGFORMULA     "Formula"
#define MSGAUTOCALC    "AutoCalc"
#define MSGFORMDISPLAY "Form"
#define MSGFILENAME    "Enter the file name of the spreadsheet:"
#define MSGCOLWIDTH    "Enter the new column width:"
#define MSGNOOPEN      "Can't open the file."
#define MSGOVERWRITE   "The file exists.  Do you want to overwrite it?"
#define MSGFILELOMEM   "Not enough memory for entire spreadsheet."
#define MSGNOTURBOCALC "That is not a Turbo C++ TurboCalc spreadsheet."
#define MSGNOEXIST     "The file does not exist."
#define MSGGOTO        "Enter the cell to go to:"
#define MSGBADNUMBER   "You must enter a number from %d to %d."
#define MSGBADCELL     "That is not a legal cell."
#define MSGCELL1       "Enter the first cell to format:"
#define MSGCELL2       "Enter the last cell to format:"
#define MSGDIFFCOLROW  "The row or the column must be the same."
#define MSGRIGHTJUST   "Do you want the cell right-justified?"
#define MSGDOLLAR      "Do you want numbers in a dollar format?"
#define MSGCOMMAS      "Do you want commas in numbers?"
#define MSGPLACES      "How many decimal places should the number be rounded to?"
#define MSGCOLUMNS     "Do you want to print in 132 columns?"
#define MSGPRINT       "Enter the file name to print to, or press ENTER to print on the printer."
#define MSGBORDER      "Print the border?"
#define MSGLOADING     "Loading..."
#define MSGSAVING      "Saving..."
#define MSGSAVESHEET   "Save current spreadsheet?"

#define MENU     "Spreadsheet, Format, Delete, Goto, Col, Row, Edit, Utility, Auto, Quit"
#define COMMAND  "SFDGCREUAQ"
#define SMENU    "Load, Save, Print, Clear"
#define SCOMMAND "LSPC"
#define CMENU    "Insert, Delete, Width"
#define CCOMMAND "IDW"
#define RMENU    "Insert, Delete"
#define RCOMMAND "ID"
#define UMENU    "Recalc, Formula display"
#define UCOMMAND "RF"

// TUI-specific dimensions (spreadsheet.h has MAXCOLS, MAXROWS, DEFAULTWIDTH, DEFAULTFORMAT,
// MAXINPUT, MAXPLACES, PARSERSTACKSIZE, MAXCOLWIDTH)
#define LEFTMARGIN  3
#define MINCOLWIDTH 3
#define SCREENCOLS  (80 - LEFTMARGIN) / MINCOLWIDTH + 1
#define SCREENROWS  20
#define TOPMARGIN   5

#define HIGHLIGHT   true
#define NOHIGHLIGHT false
#define UPDATE      true
#define NOUPDATE    false
#define FORMAT      true
#define NOFORMAT    false

// TUI-specific direction constants (spreadsheet.h has TEXT, VALUE, FORMULA, COLADD, COLDEL, ROWADD,
// ROWDEL, OVERWRITE, RJUSTIFY, COMMAS, DOLLAR, CELLREC, CELLPTR, memsize, etc.)
enum Direction {
    LEFT  = 0, //
    RIGHT = 1, //
    UP    = 2, //
    DOWN  = 3, //
};

// Control characters
enum ControlChar {
    BS       = '\b',  //
    FORMFEED = '\f',  //
    CR       = '\r',  //
    ESC      = '\33', //
};

// Key codes - will be mapped from ncurses in getkey()
enum KeyCode {
    HOMEKEY      = 1000, //
    ENDKEY       = 1001, //
    UPKEY        = 1002, //
    DOWNKEY      = 1003, //
    PGUPKEY      = 1004, //
    PGDNKEY      = 1005, //
    LEFTKEY      = 1006, //
    INSKEY       = 1007, //
    RIGHTKEY     = 1008, //
    DELKEY       = 1009, //
    CTRLLEFTKEY  = 1010, //
    CTRLRIGHTKEY = 1011, //
    F1           = 1012, //
    F2           = 1013, //
    F3           = 1014, //
    F4           = 1015, //
    F5           = 1016, //
    F6           = 1017, //
    F7           = 1018, //
    F8           = 1019, //
    F9           = 1020, //
    F10          = 1021, //
};

// TUI state structure
typedef struct {
    unsigned char colstart[SCREENCOLS];
    int leftcol, rightcol, toprow, bottomrow, curcol, currow;
    bool stop;
    unsigned int oldcursor, shortcursor, tallcursor, nocursor;
} TUIState;

void initcursor(TUIState *tui);
int getkey(void);
bool editstring(TUIState *tui, char *s, char *legal, int maxlength);
bool getint(TUIState *tui, int *number, int low, int high);
void getinput(TUIState *tui, int c);
void setcolor(int color);
void scroll_area(int direction, int lines, int x1, int y1, int x2, int y2, int attrib);
void setcursor(unsigned int shape);
void writef(int col, int row, int color, int width, char *format, ...);
void printcol(TUIState *tui);
void printrow(TUIState *tui);
void displaycell(TUIState *tui, int col, int row, int highlighting, int updating);
void displaycol(TUIState *tui, int col, int updating);
void displayrow(TUIState *tui, int row, int updating);
void displayscreen(TUIState *tui, int updating);
void clearinput(void);
void changecursor(int insmode);
void showcelltype(TUIState *tui);
void initcolortable(void);
double parse(char *s, int *att);
void printfreemem(void);
void moverowup(TUIState *tui);
void moverowdown(TUIState *tui);
void movecolleft(TUIState *tui);
void movecolright(TUIState *tui);
void recalc(TUIState *tui);
void changeautocalc(TUIState *tui, int newmode);
void changeformdisplay(int newmode);
void errormsg(char *s);
void colstring(int col, char *colstr);
void centercolstring(int col, char *colstr);
void setleftcol(TUIState *tui);
void setrightcol(TUIState *tui);
void settoprow(TUIState *tui);
void setbottomrow(TUIState *tui);
void movehighlight(void);
void act(TUIState *tui, char *s);
int getcommand(char *msgstr, char *comstr);
void mainmenu(TUIState *tui);
void editcell(TUIState *tui, CELLPTR ecell);
int setoflags(TUIState *tui, int col, int row, int display);
void clearoflags(TUIState *tui, int col, int row, int display);
void updateoflags(TUIState *tui, int col, int row, int display);
void loadsheet(TUIState *tui, char *filename);
bool getcell(TUIState *tui, int *col, int *row);
char *cellstring(int col, int row, int *color, int formatting);
void writeprompt(char *prompt);
bool getyesno(TUIState *tui, int *yesno, char *prompt);
void swap(int *val1, int *val2);
void redrawscreen(TUIState *tui);
void checkforsave(TUIState *tui);
void savesheet(TUIState *tui);
bool formulastart(char **input, int *col, int *row);
int rowwidth(int row);
void clearlastcol(TUIState *tui);
void run(TUIState *tui);
void gotocell(TUIState *tui);

#endif // TCALC_H
