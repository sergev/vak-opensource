// Turbo C++ - (C) Copyright 1987-1991 by Borland International
// Converted to ncurses

#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spreadsheet.h"
#include "tcalc.h"

//
// Uses ncurses to read the next keyboard character
//
int getkey(void)
{
    int key;

    key = getch();

    // Map ncurses special keys to DOS key codes
    if (key == KEY_UP)
        return UPKEY;
    if (key == KEY_DOWN)
        return DOWNKEY;
    if (key == KEY_LEFT)
        return LEFTKEY;
    if (key == KEY_RIGHT)
        return RIGHTKEY;
    if (key == KEY_HOME)
        return HOMEKEY;
    if (key == KEY_END)
        return ENDKEY;
    if (key == KEY_PPAGE)
        return PGUPKEY;
    if (key == KEY_NPAGE)
        return PGDNKEY;
    if (key == KEY_IC)
        return INSKEY;
    if (key == KEY_DC)
        return DELKEY;
    if (key == KEY_F(1))
        return F1;
    if (key == KEY_F(2))
        return F2;
    if (key == KEY_F(3))
        return F3;
    if (key == KEY_F(4))
        return F4;
    if (key == KEY_F(5))
        return F5;
    if (key == KEY_F(6))
        return F6;
    if (key == KEY_F(7))
        return F7;
    if (key == KEY_F(8))
        return F8;
    if (key == KEY_F(9))
        return F9;
    if (key == KEY_F(10))
        return F10;

    // Map Enter key variants to CR (Carriage Return)
    // Enter can produce '\n' (10), '\r' (13), or KEY_ENTER depending on terminal
    if (key == '\n' || key == '\r')
        return CR;
#ifdef KEY_ENTER
    if (key == KEY_ENTER)
        return CR;
#endif

    // Handle Ctrl+Left and Ctrl+Right - these come as escape sequences
    // For now, map Ctrl+Left/Right to regular Left/Right
    // Can be enhanced later if needed

    return key;
}

//
// Allows the user to edit a string with only certain characters allowed -
// Returns true if ESC was not pressed, false is ESC was pressed.
//
bool editstring(TUIState *tui, char *s, char *legal, int maxlength)
{
    int c, len = strlen(s), pos = len;
    bool insert = true;

    changecursor(insert);
    do {
        writef(1, 25, LOWCOMMANDCOLOR, 79, "%s", s);
        move(24, pos); // Row 25 in 1-based = row 24 in 0-based
        switch (c = getkey()) {
        case HOMEKEY:
            pos = 0;
            break;
        case ENDKEY:
            pos = len;
            break;
        case INSKEY:
            insert = !insert;
            changecursor(insert);
            break;
        case LEFTKEY:
            if (pos > 0)
                pos--;
            break;
        case RIGHTKEY:
            if (pos < len)
                pos++;
            break;
        case BS:
            if (pos > 0) {
                memmove(&s[pos - 1], &s[pos], len - pos + 1);
                pos--;
                len--;
            }
            break;
        case DELKEY:
            if (pos < len) {
                memmove(&s[pos], &s[pos + 1], len - pos);
                len--;
            }
            break;
        case CR:
            break;
        case UPKEY:
            c = CR;
            break;
        case DOWNKEY:
            c = CR;
            break;
        case ESC:
            len = 0;
            break;
        default:
            if (((legal[0] == 0) || (strchr(legal, c) != NULL)) && ((c >= ' ') && (c <= '~')) &&
                (len < maxlength)) {
                if (insert) {
                    memmove(&s[pos + 1], &s[pos], len - pos + 1);
                    len++;
                } else if (pos >= len)
                    len++;
                s[pos++] = c;
            }
            break;
        }
        s[len] = 0;
    } while ((c != CR) && (c != ESC));
    clearinput();
    changecursor(false);
    setcursor(tui->nocursor);
    return (c != ESC);
}

//
// Acts on a particular input
//
void act(TUIState *tui, char *s)
{
    int attrib;
    bool allocated;
    double value;
    Spreadsheet *sheet = get_spreadsheet();
    char input_copy[MAXINPUT + 1];

    // Make a copy since parse_formula modifies the input
    strcpy(input_copy, s);

    // Delete existing cell using spreadsheet API
    spreadsheet_delete_cell(sheet, tui->curcol, tui->currow);
    // Update display - cell is already freed by spreadsheet_delete_cell
    if (sheet->cell[tui->curcol][tui->currow] == NULL) {
        clearoflags(tui, tui->curcol + 1, tui->currow, UPDATE);
        updateoflags(tui, tui->curcol, tui->currow, UPDATE);
        if (UPDATE)
            displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
    }

    // Parse the input to determine type
    value = spreadsheet_parse_formula(sheet, input_copy, &attrib);

    switch (attrib) {
    case TEXT:
        allocated = spreadsheet_set_cell_text(sheet, tui->curcol, tui->currow, input_copy);
        if (allocated) {
            displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
        }
        break;
    case VALUE:
        allocated = spreadsheet_set_cell_value(sheet, tui->curcol, tui->currow, value);
        if (allocated) {
            displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
        }
        break;
    case FORMULA:
        // input_copy already contains the parsed formula string
        allocated = spreadsheet_set_cell_formula(sheet, tui->curcol, tui->currow, input_copy);
        if (allocated) {
            displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
        }
        break;
    }
    if (allocated) {
        sheet->format[tui->curcol][tui->currow] &= ~OVERWRITE;
        clearoflags(tui, tui->curcol + 1, tui->currow, UPDATE);
        if (attrib == TEXT)
            setoflags(tui, tui->curcol, tui->currow, UPDATE);
        // Update spreadsheet's lastcol/lastrow
        spreadsheet_set_last_col(sheet);
        spreadsheet_set_last_row(sheet);
        if (spreadsheet_get_autocalc(sheet))
            recalc(tui);
    } else
        errormsg(MSGLOMEM);
    printfreemem();
}

//
// Reads and acts on an input string from the keyboard that started with c.
//
void getinput(TUIState *tui, int c)
{
    char s[MAXINPUT + 1];
    Spreadsheet *sheet = get_spreadsheet();

    s[0] = c;
    s[1] = 0;
    if (!editstring(tui, s, "", MAXINPUT) || (s[0] == 0))
        return;
    act(tui, s);
    spreadsheet_set_changed(sheet, true);
    moverowdown(tui);
}

//
// Reads in a positive integer from low to high
//
bool getint(TUIState *tui, int *number, int low, int high)
{
    int i;
    bool good = false;
    char s[5], message[81];

    s[0] = 0;
    sprintf(message, MSGBADNUMBER, low, high);
    do {
        if (!editstring(tui, s, "1234567890", 4))
            return (false);
        i = atoi(s);
        if ((good = ((i >= low) && (i <= high))) == 0)
            errormsg(message);
    } while (!good);
    *number = i;
    return (true);
}

//
// Reads in a cell name that was typed in - Returns false if ESC was pressed
//
bool getcell(TUIState *tui, int *col, int *row)
{
    int len, numlen = rowwidth(MAXROWS), oldcol = *col, oldrow = *row;
    bool first = true, good = false;
    char data[10], *input, *start, numstring[6];

    data[0] = 0;
    do {
        if (!first)
            errormsg(MSGBADCELL);
        first = false;
        input = data;
        if (!editstring(tui, data, "", numlen + 2)) {
            *col = oldcol;
            *row = oldrow;
            return (false);
        }
        *col = toupper(*(input++)) - 'A';
        if (isalpha(*input)) {
            *col *= 26;
            *col += toupper(*(input++)) - 'A' + 26;
        }
        if (*col >= MAXCOLS)
            continue;
        start = input;
        for (len = 0; len < numlen; len++) {
            if (!isdigit(*(input++))) {
                input--;
                break;
            }
        }
        if (len == 0)
            continue;
        strncpy(numstring, start, len);
        numstring[len] = 0;
        *row           = atoi(numstring) - 1;
        if ((*row >= MAXROWS) || (*row == -1) || (*input != 0))
            continue;
        good = true;
    } while (!good);
    return (true);
}

//
// Prints a prompt and gets a yes or no answer - returns true if ESC was
// pressed, false if not.
//
bool getyesno(TUIState *tui, int *yesno, char *prompt)
{
    writeprompt(prompt);
    setcursor(tui->shortcursor);
    do {
        *yesno = toupper(getkey());
        if (*yesno == ESC) {
            setcursor(tui->nocursor);
            return (false);
        }
    } while (strchr("YN", *yesno) == NULL);
    setcursor(tui->nocursor);
    return (true);
}
