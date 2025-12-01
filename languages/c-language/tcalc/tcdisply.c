// Turbo C++ - (C) Copyright 1987-1991 by Borland International
// Converted to ncurses

#include <math.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spreadsheet.h"
#include "tcalc.h"

static int colortable[256];

//
// Sets the current color using the color table
//
void setcolor(int color)
{
    if (color >= 0 && color < 256) {
        attron(COLOR_PAIR(colortable[color]));
    }
}

//
// Prints a string in video memory at a selected location in a color
//
void writef(int col, int row, int color, int width, char *format, ...)
{
    va_list arg_ptr;
    char output[81];
    int len, i;

    va_start(arg_ptr, format);
    vsprintf(output, format, arg_ptr);
    output[width] = 0;
    if ((len = strlen(output)) < width) {
        for (i = len; i < width; i++)
            output[i] = ' ';
        output[width] = 0;
    }
    setcolor(color);
    move(row - 1, col - 1); // ncurses uses 0-based, row-first
    addstr(output);
    refresh();
}

//
// Scrolls an area of the screen
//
void scroll_area(int direction, int lines, int x1, int y1, int x2, int y2, int attrib)
{
    int i, j;
    chtype ch;

    if (lines == 0) {
        // Clear area
        setcolor(attrib);
        for (i = y1 - 1; i < y2; i++) {
            for (j = x1 - 1; j < x2; j++) {
                move(i, j);
                addch(' ');
            }
        }
        refresh();
    } else {
        switch (direction) {
        case UP:
            // Move content up
            for (i = y1 - 1; i < y2 - lines; i++) {
                for (j = x1 - 1; j < x2; j++) {
                    ch = mvinch(i + lines, j);
                    move(i, j);
                    addch(ch);
                }
            }
            // Clear bottom area
            setcolor(attrib);
            for (i = y2 - lines; i < y2; i++) {
                for (j = x1 - 1; j < x2; j++) {
                    move(i, j);
                    addch(' ');
                }
            }
            break;
        case DOWN:
            // Move content down
            for (i = y2 - 1; i >= y1 - 1 + lines; i--) {
                for (j = x1 - 1; j < x2; j++) {
                    ch = mvinch(i - lines, j);
                    move(i, j);
                    addch(ch);
                }
            }
            // Clear top area
            setcolor(attrib);
            for (i = y1 - 1; i < y1 - 1 + lines; i++) {
                for (j = x1 - 1; j < x2; j++) {
                    move(i, j);
                    addch(' ');
                }
            }
            break;
        case LEFT:
            // Move content left
            for (i = y1 - 1; i < y2; i++) {
                for (j = x1 - 1; j < x2 - lines; j++) {
                    ch = mvinch(i, j + lines);
                    move(i, j);
                    addch(ch);
                }
            }
            // Clear right area
            setcolor(attrib);
            for (i = y1 - 1; i < y2; i++) {
                for (j = x2 - lines; j < x2; j++) {
                    move(i, j);
                    addch(' ');
                }
            }
            break;
        case RIGHT:
            // Move content right
            for (i = y1 - 1; i < y2; i++) {
                for (j = x2 - 1; j >= x1 - 1 + lines; j--) {
                    ch = mvinch(i, j - lines);
                    move(i, j);
                    addch(ch);
                }
            }
            // Clear left area
            setcolor(attrib);
            for (i = y1 - 1; i < y2; i++) {
                for (j = x1 - 1; j < x1 - 1 + lines; j++) {
                    move(i, j);
                    addch(' ');
                }
            }
            break;
        }
        refresh();
    }
}

//
// Sets the shape of the cursor - shape 0x2000 means invisible
//
void setcursor(unsigned int shape)
{
    if (shape == 0x2000) {
        curs_set(0); // Invisible
    } else {
        curs_set(1); // Normal
    }
}

//
// Returns the shape of the current cursor - not used in ncurses
//
unsigned int getcursor(void)
{
    return (0x0607); // Return default shape
}

//
// Changes the cursor shape based on the current insert mode
//
void changecursor(int insmode)
{
    if (insmode)
        curs_set(2); // Very visible for insert mode
    else
        curs_set(1); // Normal for overwrite mode
}

//
// Prints the column headings
//
void printcol(TUIState *tui)
{
    int col;
    char colstr[MAXCOLWIDTH + 1];
    Spreadsheet *sheet = get_spreadsheet();

    scroll_area(UP, 0, 1, 2, 80, 2, HEADERCOLOR);
    for (col = tui->leftcol; col <= tui->rightcol; col++) {
        centercolstring(col, colstr);
        writef(tui->colstart[col - tui->leftcol] + 1, 2, HEADERCOLOR, sheet->colwidth[col], colstr);
    }
}

//
// Clears any data left in the last column
//
void clearlastcol(TUIState *tui)
{
    int col;
    Spreadsheet *sheet = get_spreadsheet();

    if ((col = tui->colstart[tui->rightcol - tui->leftcol] + sheet->colwidth[tui->rightcol]) < 80)
        scroll_area(UP, 0, col + 1, 3, 80, SCREENROWS + 2, TEXTCOLOR);
}

//
// Prints the row headings
//
void printrow(TUIState *tui)
{
    int row;

    for (row = 0; row < SCREENROWS; row++)
        writef(1, row + 3, HEADERCOLOR, LEFTMARGIN, "%-d", row + tui->toprow + 1);
}

//
// Displays the contents of a cell
//
void displaycell(TUIState *tui, int col, int row, int highlighting, int updating)
{
    int color;
    char *s;
    Spreadsheet *sheet = get_spreadsheet();

    if ((updating) &&
        ((sheet->cell[col][row] == NULL) || (sheet->cell[col][row]->attrib != FORMULA)))
        return;
    s = cellstring(col, row, &color, FORMAT);
    if (highlighting) {
        if (color == ERRORCOLOR)
            color = HIGHLIGHTERRORCOLOR;
        else
            color = HIGHLIGHTCOLOR;
    }
    writef(tui->colstart[col - tui->leftcol] + 1, row - tui->toprow + 3, color,
           sheet->colwidth[col], "%s", s);
}

//
// Displays a column on the screen
//
void displaycol(TUIState *tui, int col, int updating)
{
    int row;

    for (row = tui->toprow; row <= tui->bottomrow; row++)
        displaycell(tui, col, row, NOHIGHLIGHT, updating);
}

//
// Displays a row on the screen
//
void displayrow(TUIState *tui, int row, int updating)
{
    int col;

    for (col = tui->leftcol; col <= tui->rightcol; col++)
        displaycell(tui, col, row, NOHIGHLIGHT, updating);
}

//
// Displays the current screen of the spreadsheet
//
void displayscreen(TUIState *tui, int updating)
{
    int row;

    for (row = tui->toprow; row <= tui->bottomrow; row++)
        displayrow(tui, row, updating);
    clearlastcol(tui);
}

//
// Clears the input line
//
void clearinput(void)
{
    int i;
    setcolor(LOWCOMMANDCOLOR);
    for (i = 0; i < 80; i++) {
        move(24, i);
        addch(' ');
    }
    move(24, 0);
    refresh();
}

//
// Prints the type of cell and what is in it
//
void showcelltype(TUIState *tui)
{
    char colstr[3], *s;
    int color;
    Spreadsheet *sheet = get_spreadsheet();
    CELLPTR curcell    = sheet->cell[tui->curcol][tui->currow];
    bool formdisplay   = spreadsheet_get_formdisplay(sheet);

    spreadsheet_set_formdisplay(sheet, !formdisplay);
    s = cellstring(tui->curcol, tui->currow, &color, NOFORMAT);
    colstring(tui->curcol, colstr);
    if (curcell == NULL)
        writef(1, 23, CELLTYPECOLOR, 80, "%s%d %s", colstr, tui->currow + 1, MSGEMPTY);
    else
        switch (curcell->attrib) {
        case TEXT:
            writef(1, 23, CELLTYPECOLOR, 80, "%s%d %s", colstr, tui->currow + 1, MSGTEXT);
            break;
        case VALUE:
            writef(1, 23, CELLTYPECOLOR, 80, "%s%d %s", colstr, tui->currow + 1, MSGVALUE);
            break;
        case FORMULA:
            writef(1, 23, CELLTYPECOLOR, 80, "%s%d %s", colstr, tui->currow + 1, MSGFORMULA);
            break;
        }
    writef(1, 24, CELLCONTENTSCOLOR, 80, "%s", s);
    spreadsheet_set_formdisplay(sheet, formdisplay);
}

//
// Displays the entire screen
//
void redrawscreen(TUIState *tui)
{
    Spreadsheet *sheet = get_spreadsheet();
    setrightcol(tui);
    setbottomrow(tui);
    writef(1, 1, MSGMEMORYCOLOR, strlen(MSGMEMORY), MSGMEMORY);
    writef(29, 1, PROMPTCOLOR, strlen(MSGCOMMAND), MSGCOMMAND);
    changeautocalc(tui, spreadsheet_get_autocalc(sheet));
    changeformdisplay(spreadsheet_get_formdisplay(sheet));
    printfreemem();
    displayscreen(tui, NOUPDATE);
}

//
// Initializes the different cursor types
//
void initcursor(TUIState *tui)
{
    tui->oldcursor   = 0x0607; // Default cursor shape
    tui->shortcursor = 0x0607;
    tui->tallcursor  = 0x0507;
}

//
// Wrapper for spreadsheet_cell_to_string - converts column number to string
//
void colstring(int col, char *colstr)
{
    spreadsheet_cell_to_string(col, colstr);
}

//
// Changes a column to a centered string
//
void centercolstring(int col, char *colstr)
{
    char s[3];
    int spaces1, spaces2;
    Spreadsheet *sheet = get_spreadsheet();

    colstring(col, s);
    spaces1 = (sheet->colwidth[col] - strlen(s)) >> 1;
    spaces2 = sheet->colwidth[col] - strlen(s) - spaces1;
    sprintf(colstr, "%*s%s%*s", spaces1, "", s, spaces2, "");
}

//
// Sets the value of leftcol based on the value of rightcol
//
void setleftcol(TUIState *tui)
{
    int total = 80, col = 0;
    Spreadsheet *sheet = get_spreadsheet();

    while ((total >= LEFTMARGIN) && (tui->rightcol - col >= 0)) {
        tui->colstart[SCREENCOLS - col - 1] = total - sheet->colwidth[tui->rightcol - col];
        total -= sheet->colwidth[tui->rightcol - col++];
    }
    if (total >= LEFTMARGIN)
        col++;
    memmove(tui->colstart, &tui->colstart[SCREENCOLS - col + 1], col - 1);
    tui->leftcol = tui->rightcol - col + 2;
    total        = tui->colstart[0] - LEFTMARGIN;
    if (total != 0) {
        for (col = tui->leftcol; col <= tui->rightcol; col++)
            tui->colstart[col - tui->leftcol] -= total;
    }
    printcol(tui);
}

//
// Sets the value of rightcol based on the value of leftcol
//
void setrightcol(TUIState *tui)
{
    int total = LEFTMARGIN, col = 0;
    Spreadsheet *sheet = get_spreadsheet();

    do {
        tui->colstart[col] = total;
        total += sheet->colwidth[tui->leftcol + col++];
    } while ((total <= 80) && (tui->leftcol + col <= MAXCOLS));
    tui->rightcol = tui->leftcol + col - 2;
    printcol(tui);
}

//
// Figures out the value of toprow based on the value of bottomrow
//
void settoprow(TUIState *tui)
{
    if (tui->bottomrow - SCREENROWS < -1)
        tui->bottomrow = 19;
    tui->toprow = tui->bottomrow - 19;
    printrow(tui);
}

//
// Figures out the value of bottomrow based on the value of toprow
//
void setbottomrow(TUIState *tui)
{
    if (tui->toprow + SCREENROWS > MAXROWS)
        tui->toprow = MAXROWS - 20;
    tui->bottomrow = tui->toprow + 19;
    printrow(tui);
}

//
// Sets the overwrite flag on cells starting at (col + 1, row) - returns
// the number of the column after the last column set.
//
int setoflags(TUIState *tui, int col, int row, int display)
{
    int len;
    Spreadsheet *sheet = get_spreadsheet();

    len = strlen(sheet->cell[col][row]->v.text) - sheet->colwidth[col];
    while ((++col < MAXCOLS) && (len > 0) && (sheet->cell[col][row] == NULL)) {
        sheet->format[col][row] |= OVERWRITE;
        len -= sheet->colwidth[col];
        if (display && (col >= tui->leftcol) && (col <= tui->rightcol))
            displaycell(tui, col, row, NOHIGHLIGHT, NOUPDATE);
    }
    return (col);
}

//
// Clears the overwrite flag on cells starting at (col, row)
//
void clearoflags(TUIState *tui, int col, int row, int display)
{
    Spreadsheet *sheet = get_spreadsheet();
    while ((sheet->format[col][row] >= OVERWRITE) && (col < MAXCOLS) &&
           (sheet->cell[col][row] == NULL)) {
        sheet->format[col][row] &= ~OVERWRITE;
        if (display && (col >= tui->leftcol) && (col <= tui->rightcol))
            displaycell(tui, col, row, NOHIGHLIGHT, NOUPDATE);
        col++;
    }
}

//
// Starting in col, moves back to the last TEXT cell and updates all flags
//
void updateoflags(TUIState *tui, int col, int row, int display)
{
    Spreadsheet *sheet = get_spreadsheet();
    while ((sheet->cell[col][row] == NULL) && (col-- > 0))
        ;
    if ((sheet->cell[col][row] != NULL) && (sheet->cell[col][row]->attrib == TEXT) && (col >= 0))
        setoflags(tui, col, row, display);
}

//
// Sets the string representation of text
//
void textstring(char *instring, char *outstring, int col, int fvalue, int formatting)
{
    char *just, *ljust = "%-*s", *rjust = "%*s";
    Spreadsheet *sheet = get_spreadsheet();

    if ((fvalue & RJUSTIFY) && (formatting))
        just = rjust;
    else
        just = ljust;
    sprintf(outstring, just, sheet->colwidth[col], instring);
    if (formatting)
        outstring[sheet->colwidth[col]] = 0;
}

//
// Sets the string representation of a value
//
void valuestring(CELLPTR cellptr, double value, char *vstring, int col, int fvalue, int *color,
                 int formatting)
{
    char s[81];
    char *fstring;
    int width, pos;

    if (value == HUGE_VAL) {
        strcpy(vstring, MSGERROR);
        *color = ERRORCOLOR;
    } else {
        if (formatting) {
            sprintf(vstring, "%1.*f", fvalue & 15, cellptr->v.value);
            if (fvalue & COMMAS) {
                pos = strcspn(vstring, ".");
                while (pos > 3) {
                    pos -= 3;
                    if (vstring[pos - 1] != '-') {
                        memmove(&vstring[pos + 1], &vstring[pos], strlen(vstring) - pos + 1);
                        vstring[pos] = ',';
                    }
                }
            }
            if (fvalue & DOLLAR) {
                Spreadsheet *sheet = get_spreadsheet();
                if (vstring[0] == '-') {
                    fstring = " $";
                    width   = sheet->colwidth[col] - 2;
                } else {
                    fstring = " $ ";
                    width   = sheet->colwidth[col] - 3;
                }
            } else {
                Spreadsheet *sheet = get_spreadsheet();
                fstring            = "";
                width              = sheet->colwidth[col];
            }
            strcpy(s, vstring);
            if (fvalue & RJUSTIFY) {
                if ((int)strlen(vstring) > width)
                    vstring[width] = 0;
                else
                    sprintf(vstring, "%*s", width, s);
            } else
                sprintf(vstring, "%-*s", width, s);
            memmove(&vstring[strlen(fstring)], vstring, strlen(vstring) + 1);
            memcpy(vstring, fstring, strlen(fstring));
        } else
            sprintf(vstring, "%.*f", MAXPLACES, value);
        *color = VALUECOLOR;
    }
}

//
// Creates an output string for the data in the cell in (col, row), and
// also returns the color of the cell
//
char *cellstring(int col, int row, int *color, int formatting)
{
    Spreadsheet *sheet = get_spreadsheet();
    CELLPTR cellptr    = sheet->cell[col][row];
    int newcol, formatvalue;
    static char s[81], temp[MAXCOLWIDTH + 1];
    char *p;
    double value;

    if (cellptr == NULL) {
        if (!formatting || (sheet->format[col][row] < OVERWRITE)) {
            sprintf(s, "%*s", sheet->colwidth[col], "");
            *color = BLANKCOLOR;
        } else {
            newcol = col;
            while (sheet->cell[--newcol][row] == NULL)
                ;
            p = sheet->cell[newcol][row]->v.text;
            while (newcol < col)
                p += sheet->colwidth[newcol++];
            strncpy(temp, p, sheet->colwidth[col]);
            temp[sheet->colwidth[col]] = 0;
            sprintf(s, "%s%*s", temp, (int)(sheet->colwidth[col] - strlen(temp)), "");
            *color = TEXTCOLOR;
        }
    } else {
        formatvalue = sheet->format[col][row];
        switch (cellptr->attrib) {
        case TEXT:
            textstring(cellptr->v.text, s, col, formatvalue, formatting);
            *color = TEXTCOLOR;
            break;
        case FORMULA:
            if (spreadsheet_get_formdisplay(sheet)) {
                textstring(cellptr->v.f.formula, s, col, formatvalue, formatting);
                *color = FORMULACOLOR;
                break;
            } else
                value = cellptr->v.f.fvalue;
        case VALUE:
            if (cellptr->attrib == VALUE)
                value = cellptr->v.value;
            valuestring(cellptr, value, s, col, formatvalue, color, formatting);
            break;
        }
    }
    return (s);
}

//
// Prints the amount of free memory
//
void printfreemem(void)
{
    Spreadsheet *sheet = get_spreadsheet();
    writef(strlen(MSGMEMORY) + 2, 1, MEMORYCOLOR, 6, "%6ld", spreadsheet_get_memleft(sheet));
}

//
// Prints an error message at the bottom of the screen
//
void errormsg(char *s)
{
    beep(); // Beeps the speaker
    writef(1, 25, ERRORCOLOR, 79, "%s  %s", s, MSGKEYPRESS);
    move(24, strlen(s) + strlen(MSGKEYPRESS) + 2); // Row 25 in 1-based = row 24 in 0-based
    refresh();
    getkey();
    move(24, 0);
    writef(1, 25, LOWCOMMANDCOLOR, 79, "");
}

//
// Prints a prompt on the screen
//
void writeprompt(char *prompt)
{
    writef(1, 24, PROMPTCOLOR, 80, prompt);
}

//
// Sets up the color table - maps DOS colors to ncurses color pairs
//
void initcolortable(void)
{
    int i;

    // Initialize ncurses colors
    start_color();

    // Define color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK);    // TEXTCOLOR
    init_pair(2, COLOR_RED, COLOR_BLACK);      // ERRORCOLOR
    init_pair(3, COLOR_CYAN, COLOR_BLACK);     // VALUECOLOR
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);  // FORMULACOLOR
    init_pair(5, COLOR_BLACK, COLOR_BLACK);    // BLANKCOLOR
    init_pair(6, COLOR_WHITE, COLOR_RED);      // HEADERCOLOR
    init_pair(7, COLOR_WHITE, COLOR_BLUE);     // HIGHLIGHTCOLOR
    init_pair(8, COLOR_WHITE, COLOR_BLUE);     // HIGHLIGHTERRORCOLOR
    init_pair(9, COLOR_CYAN, COLOR_BLACK);     // MSGAUTOCALCCOLOR
    init_pair(10, COLOR_MAGENTA, COLOR_BLACK); // MSGFORMDISPLAYCOLOR
    init_pair(11, COLOR_GREEN, COLOR_BLACK);   // MSGMEMORYCOLOR
    init_pair(12, COLOR_CYAN, COLOR_BLACK);    // MSGHEADERCOLOR
    init_pair(13, COLOR_YELLOW, COLOR_BLACK);  // PROMPTCOLOR
    init_pair(14, COLOR_CYAN, COLOR_BLACK);    // COMMANDCOLOR
    init_pair(15, COLOR_WHITE, COLOR_BLACK);   // LOWCOMMANDCOLOR
    init_pair(16, COLOR_RED, COLOR_BLACK);     // MEMORYCOLOR
    init_pair(17, COLOR_GREEN, COLOR_BLACK);   // CELLTYPECOLOR
    init_pair(18, COLOR_YELLOW, COLOR_BLACK);  // CELLCONTENTSCOLOR

    // Map color codes to color pairs
    colortable[TEXTCOLOR]           = 1;
    colortable[ERRORCOLOR]          = 2;
    colortable[VALUECOLOR]          = 3;
    colortable[FORMULACOLOR]        = 4;
    colortable[BLANKCOLOR]          = 5;
    colortable[HEADERCOLOR]         = 6;
    colortable[HIGHLIGHTCOLOR]      = 7;
    colortable[HIGHLIGHTERRORCOLOR] = 8;
    colortable[MSGAUTOCALCCOLOR]    = 9;
    colortable[MSGFORMDISPLAYCOLOR] = 10;
    colortable[MSGMEMORYCOLOR]      = 11;
    colortable[MSGHEADERCOLOR]      = 12;
    colortable[PROMPTCOLOR]         = 13;
    colortable[COMMANDCOLOR]        = 14;
    colortable[LOWCOMMANDCOLOR]     = 15;
    colortable[MEMORYCOLOR]         = 16;
    colortable[CELLTYPECOLOR]       = 17;
    colortable[CELLCONTENTSCOLOR]   = 18;

    // Initialize rest of table
    for (i = 0; i < 256; i++) {
        if (colortable[i] == 0)
            colortable[i] = 1; // Default to white on black
    }
}
