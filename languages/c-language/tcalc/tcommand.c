// Turbo C++ - (C) Copyright 1987-1991 by Borland International
// Converted to ncurses

#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tcalc.h"

//
// Reads in a command and acts on it
//
int getcommand(char *msgstr, char *comstr)
{
    int ch, counter, len = strlen(msgstr);

    scroll_area(UP, 0, 1, 24, 80, 24, TEXTCOLOR);
    for (counter = 0; counter < len; counter++) {
        if (isupper(msgstr[counter]))
            writef(counter + 1, 24, COMMANDCOLOR, 1, "%c", msgstr[counter]);
        else
            writef(counter + 1, 24, LOWCOMMANDCOLOR, 1, "%c", msgstr[counter]);
    }
    do
        ch = toupper(getkey());
    while ((strchr(comstr, ch) == NULL) && (ch != ESC));
    clearinput();
    return ((ch == ESC) ? -1 : strlen(comstr) - strlen(strchr(comstr, ch)));
}

//
// If the spreadsheet has been changed, will ask the user if they want to
// save it.
//
void checkforsave(TUIState *tui)
{
    int save;
    Spreadsheet *sheet = get_spreadsheet();

    if (spreadsheet_is_changed(sheet) && getyesno(tui, &save, MSGSAVESHEET) && (save == 'Y'))
        savesheet(tui);
}

//
// Moves up 1 row
//
void moverowup(TUIState *tui)
{
    displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
    if (tui->currow > tui->toprow)
        tui->currow--;
    else if (tui->toprow != 0) {
        scroll_area(DOWN, 1, LEFTMARGIN + 1, 3, 80, SCREENROWS + 2, TEXTCOLOR);
        displayrow(tui, --tui->toprow, NOUPDATE);
        tui->currow--;
        setbottomrow(tui);
    }
}

//
// Moves down one row
//
void moverowdown(TUIState *tui)
{
    displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
    if (tui->currow < tui->bottomrow)
        tui->currow++;
    else if (tui->bottomrow < (MAXROWS - 1)) {
        scroll_area(UP, 1, LEFTMARGIN + 1, 3, 80, SCREENROWS + 2, TEXTCOLOR);
        tui->toprow++;
        tui->currow++;
        setbottomrow(tui);
        displayrow(tui, tui->bottomrow, NOUPDATE);
    }
}

//
// Moves left one column
//
void movecolleft(TUIState *tui)
{
    int col, oldleftcol;
    unsigned char oldcolstart[SCREENCOLS];

    oldleftcol = tui->leftcol;
    memcpy(oldcolstart, tui->colstart, sizeof(tui->colstart));
    displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
    if (tui->curcol > tui->leftcol)
        tui->curcol--;
    else if (tui->leftcol != 0) {
        tui->curcol--;
        tui->leftcol--;
        setrightcol(tui);
        setleftcol(tui);
        if (oldleftcol <= tui->rightcol)
            scroll_area(RIGHT, tui->colstart[oldleftcol - tui->leftcol] - LEFTMARGIN,
                        LEFTMARGIN + 1, 3, 80, SCREENROWS + 2, TEXTCOLOR);
        clearlastcol(tui);
        for (col = tui->leftcol; col <= oldleftcol - 1; col++)
            displaycol(tui, col, NOUPDATE);
    }
}

//
// Moves right one column
//
void movecolright(TUIState *tui)
{
    int col, oldleftcol, oldrightcol;
    unsigned char oldcolstart[SCREENCOLS];

    memcpy(oldcolstart, tui->colstart, sizeof(tui->colstart));
    oldleftcol  = tui->leftcol;
    oldrightcol = tui->rightcol;
    displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
    if (tui->curcol < tui->rightcol)
        tui->curcol++;
    else if (tui->rightcol < (MAXCOLS - 1)) {
        tui->curcol++;
        tui->rightcol++;
        setleftcol(tui);
        setrightcol(tui);
        if (oldrightcol >= tui->leftcol)
            scroll_area(LEFT, oldcolstart[tui->leftcol - oldleftcol] - LEFTMARGIN, LEFTMARGIN + 1,
                        3, 80, SCREENROWS + 2, TEXTCOLOR);
        clearlastcol(tui);
        for (col = oldrightcol + 1; col <= tui->rightcol; col++)
            displaycol(tui, col, NOUPDATE);
    }
}

//
// Recalculates all of the numbers in the speadsheet
//
void recalc(TUIState *tui)
{
    Spreadsheet *sheet = get_spreadsheet();
    spreadsheet_recalc(sheet);
    displayscreen(tui, UPDATE);
}

//
// Changes and prints the current AutoCalc value on the screen
//
void changeautocalc(TUIState *tui, int newmode)
{
    char s[15];
    Spreadsheet *sheet = get_spreadsheet();

    if (!spreadsheet_get_autocalc(sheet) && newmode)
        recalc(tui);
    spreadsheet_set_autocalc(sheet, newmode);
    if (spreadsheet_get_autocalc(sheet))
        strcpy(s, MSGAUTOCALC);
    else
        s[0] = 0;
    writef(73, 1, MSGAUTOCALCCOLOR, strlen(MSGAUTOCALC), s);
}

//
// Changes and prints the current formula display value on the screen
//
void changeformdisplay(int newmode)
{
    char s[15];
    Spreadsheet *sheet = get_spreadsheet();

    spreadsheet_set_formdisplay(sheet, newmode);
    if (spreadsheet_get_formdisplay(sheet))
        strcpy(s, MSGFORMDISPLAY);
    else
        s[0] = 0;
    writef(65, 1, MSGFORMDISPLAYCOLOR, strlen(MSGFORMDISPLAY), s);
}

//
// Edits a selected cell
//
void editcell(TUIState *tui, CELLPTR ecell)
{
    char s[MAXINPUT + 1];

    if (ecell == NULL)
        return;
    switch (ecell->attrib) {
    case TEXT:
        strcpy(s, ecell->v.text);
        break;
    case VALUE:
        if (ecell->v.value == HUGE_VAL)
            strcpy(s, "0");
        else
            sprintf(s, "%.*f", MAXPLACES, ecell->v.value);
        break;
    case FORMULA:
        strcpy(s, ecell->v.f.formula);
        break;
    }
    if (!editstring(tui, s, "", MAXINPUT) || (s[0] == 0))
        return;
    act(tui, s);
    moverowdown(tui);
}

//
// Clears the current spreadsheet
//
void clearsheet(TUIState *tui)
{
    Spreadsheet *sheet = get_spreadsheet();
    spreadsheet_clear(sheet);
    setrightcol(tui);
    setbottomrow(tui);
    displayscreen(tui, NOUPDATE);
    printfreemem();
}

//
// Loads a new spreadsheet
//
void loadsheet(TUIState *tui, char *filename)
{
    int result;
    Spreadsheet *sheet = get_spreadsheet();

    if (filename[0] == 0) {
        writeprompt(MSGFILENAME);
        if (!editstring(tui, filename, "", MAXINPUT))
            return;
    }
    writef(1, 25, PROMPTCOLOR, 79, MSGLOADING);
    move(24, strlen(MSGLOADING)); // Row 25 in 1-based = row 24 in 0-based
    result = spreadsheet_load(sheet, filename);
    writef(1, 25, LOWCOMMANDCOLOR, strlen(MSGLOADING), "");
    move(24, 0);
    printfreemem();
    if (result != 0) {
        switch (result) {
        case -1:
            errormsg(MSGNOEXIST);
            break;
        case -2:
            errormsg(MSGNOOPEN);
            break;
        case -3:
            errormsg(MSGNOTURBOCALC);
            break;
        case -4:
            errormsg(MSGFILELOMEM);
            break;
        }
        return;
    }
    tui->curcol = tui->currow = 0;
    setrightcol(tui);
    displayscreen(tui, NOUPDATE);
}

//
// Saves the current spreadsheet
//
void savesheet(TUIState *tui)
{
    char filename[MAXINPUT + 1];
    int overwrite, result;
    Spreadsheet *sheet = get_spreadsheet();

    filename[0] = 0;
    writeprompt(MSGFILENAME);
    if (!editstring(tui, filename, "", MAXINPUT))
        return;
    if (!access(filename, F_OK)) {
        if (!getyesno(tui, &overwrite, MSGOVERWRITE) || (overwrite == 'N'))
            return;
    }
    writef(1, 25, PROMPTCOLOR, 79, MSGSAVING);
    move(24, strlen(MSGSAVING)); // Row 25 in 1-based = row 24 in 0-based
    result = spreadsheet_save(sheet, filename);
    writef(1, 25, LOWCOMMANDCOLOR, strlen(MSGSAVING), "");
    move(24, 0);
    if (result != 0) {
        errormsg(MSGNOOPEN);
    }
}

//
// Returns the number of rows to print
//
int pagerows(int row, int toppage, int border)
{
    int rows;
    Spreadsheet *sheet = get_spreadsheet();

    rows = toppage ? 66 - TOPMARGIN : 66;
    if (border)
        rows--;
    if (row + rows - 1 > sheet->lastrow)
        return (sheet->lastrow - row + 1);
    else
        return (rows);
}

//
// Returns the number of columns to print starting at col
//
int pagecols(int col, int border, int columns)
{
    int len            = ((col == 0) && (border)) ? columns - LEFTMARGIN : columns;
    int firstcol       = col;
    Spreadsheet *sheet = get_spreadsheet();

    while ((len > 0) && (col <= sheet->lastcol))
        len -= sheet->colwidth[col++];
    if (len < 0)
        col--;
    return (col - firstcol);
}

//
// Prints a copy of the spreadsheet to a file or to the printer
//
void printsheet(TUIState *tui)
{
    char filename[MAXINPUT + 1], s[133], colstr[MAXCOLWIDTH + 1];
    FILE *file;
    int columns, counter1, counter2, counter3, col = 0, row, border, toppage, lcol, lrow, dummy,
                                               printed, oldlastcol;

    filename[0] = 0;
    writeprompt(MSGPRINT);
    if (!editstring(tui, filename, "", MAXINPUT))
        return;
    if (filename[0] == 0)
        strcpy(filename, "PRN");
    if ((file = fopen(filename, "wt")) == NULL) {
        errormsg(MSGNOOPEN);
        return;
    }
    Spreadsheet *sheet = get_spreadsheet();
    oldlastcol         = sheet->lastcol;
    for (counter1 = 0; counter1 <= sheet->lastrow; counter1++) {
        for (counter2 = sheet->lastcol; counter2 < MAXCOLS; counter2++) {
            if (sheet->format[counter2][counter1] >= OVERWRITE)
                spreadsheet_set_last_col(sheet);
        }
    }
    if (!getyesno(tui, &columns, MSGCOLUMNS))
        return;
    columns = (columns == 'Y') ? 131 : 79;
    if (!getyesno(tui, &border, MSGBORDER))
        return;
    border = (border == 'Y');
    while (col <= sheet->lastcol) {
        row     = 0;
        toppage = true;
        lcol    = pagecols(col, border, columns) + col;
        while (row <= sheet->lastrow) {
            lrow    = pagerows(row, toppage, border) + row;
            printed = 0;
            if (toppage) {
                for (counter1 = 0; counter1 < TOPMARGIN; counter1++) {
                    fprintf(file, "\n");
                    printed++;
                }
            }
            for (counter1 = row; counter1 < lrow; counter1++) {
                if ((border) && (counter1 == row) && (toppage)) {
                    if ((col == 0) && (border))
                        sprintf(s, "%*s", LEFTMARGIN, "");
                    else
                        s[0] = 0;
                    for (counter3 = col; counter3 < lcol; counter3++) {
                        centercolstring(counter3, colstr);
                        strcat(s, colstr);
                    }
                    fprintf(file, "%s\n", s);
                    printed++;
                }
                if ((col == 0) && (border))
                    sprintf(s, "%-*d", LEFTMARGIN, counter1 + 1);
                else
                    s[0] = 0;
                for (counter2 = col; counter2 < lcol; counter2++)
                    strcat(s, cellstring(counter2, counter1, &dummy, FORMAT));
                fprintf(file, "%s\n", s);
                printed++;
            }
            row     = lrow;
            toppage = false;
            if (printed < 66)
                fprintf(file, "%c", FORMFEED);
        }
        col = lcol;
    }
    fclose(file);
    // Restore lastcol if it was modified
    if (oldlastcol != sheet->lastcol) {
        // Recalculate lastcol properly
        spreadsheet_set_last_col(sheet);
    }
}

//
// Sets the new column width for a selected column
//
void setcolwidth(TUIState *tui, int col)
{
    int width, row;
    Spreadsheet *sheet = get_spreadsheet();

    writeprompt(MSGCOLWIDTH);
    if (!getint(tui, &width, MINCOLWIDTH, MAXCOLWIDTH))
        return;
    spreadsheet_set_col_width(sheet, col, width);
    setrightcol(tui);
    if (tui->rightcol < col) {
        tui->rightcol = col;
        setleftcol(tui);
        setrightcol(tui);
    }
    for (row = 0; row <= sheet->lastrow; row++) {
        if ((sheet->cell[col][row] != NULL) && (sheet->cell[col][row]->attrib == TEXT))
            clearoflags(tui, col + 1, row, NOUPDATE);
        else
            clearoflags(tui, col, row, NOUPDATE);
        updateoflags(tui, col, row, NOUPDATE);
    }
    displayscreen(tui, NOUPDATE);
}

//
// Moves to a selected cell
//
void gotocell(TUIState *tui)
{
    writeprompt(MSGGOTO);
    if (!getcell(tui, &tui->curcol, &tui->currow))
        return;
    tui->leftcol = tui->curcol;
    tui->toprow  = tui->currow;
    setbottomrow(tui);
    setrightcol(tui);
    setleftcol(tui);
    displayscreen(tui, NOUPDATE);
}

//
// Prompts the user for a selected format and range of cells
//
void formatcells(TUIState *tui)
{
    int col, row, col1, col2, row1, row2, temp, newformat = 0;
    Spreadsheet *sheet = get_spreadsheet();

    writeprompt(MSGCELL1);
    if (!getcell(tui, &col1, &row1))
        return;
    writeprompt(MSGCELL2);
    if (!getcell(tui, &col2, &row2))
        return;
    if ((col1 != col2) && (row1 != row2))
        errormsg(MSGDIFFCOLROW);
    else {
        if (col1 > col2)
            swap(&col1, &col2);
        if (row1 > row2)
            swap(&row1, &row2);
        if (!getyesno(tui, &temp, MSGRIGHTJUST))
            return;
        newformat += (temp == 'Y') * RJUSTIFY;
        if (!getyesno(tui, &temp, MSGDOLLAR))
            return;
        newformat += (temp == 'Y') * DOLLAR;
        if (!getyesno(tui, &temp, MSGCOMMAS))
            return;
        newformat += (temp == 'Y') * COMMAS;
        if (newformat & DOLLAR)
            newformat += 2;
        else {
            writeprompt(MSGPLACES);
            if (!getint(tui, &temp, 0, MAXPLACES))
                return;
            newformat += temp;
        }
        spreadsheet_format_cells(sheet, col1, row1, col2, row2, newformat);
        for (col = col1; col <= col2; col++) {
            for (row = row1; row <= row2; row++) {
                if ((col >= tui->leftcol) && (col <= tui->rightcol) && (row >= tui->toprow) &&
                    (row <= tui->bottomrow))
                    displaycell(tui, col, row, NOHIGHLIGHT, NOUPDATE);
            }
        }
    }
}

//
// Deletes a column
//
void deletecol(TUIState *tui, int col)
{
    int counter, row;
    Spreadsheet *sheet = get_spreadsheet();

    spreadsheet_delete_col(sheet, col);
    printfreemem();
    setrightcol(tui);
    if (tui->curcol > tui->rightcol) {
        tui->rightcol++;
        setleftcol(tui);
    }
    clearlastcol(tui);
    for (counter = 0; counter <= sheet->lastcol; counter++) {
        for (row = 0; row <= sheet->lastrow; row++) {
            updateoflags(tui, counter, row, NOUPDATE);
        }
    }
    while (col <= tui->rightcol)
        displaycol(tui, col++, NOUPDATE);
    recalc(tui);
}

//
// Inserts a column
//
void insertcol(TUIState *tui, int col)
{
    int counter, row;
    Spreadsheet *sheet = get_spreadsheet();

    spreadsheet_insert_col(sheet, col);
    setrightcol(tui);
    if (tui->curcol > tui->rightcol) {
        tui->rightcol++;
        setleftcol(tui);
    }
    for (counter = 0; counter <= sheet->lastcol; counter++) {
        for (row = 0; row <= sheet->lastrow; row++) {
            updateoflags(tui, counter, row, NOUPDATE);
        }
    }
    while (col <= tui->rightcol)
        displaycol(tui, col++, NOUPDATE);
    recalc(tui);
}

//
// Deletes a row
//
void deleterow(TUIState *tui, int row)
{
    Spreadsheet *sheet = get_spreadsheet();

    spreadsheet_delete_row(sheet, row);
    printfreemem();
    while (row <= tui->bottomrow)
        displayrow(tui, row++, NOUPDATE);
    recalc(tui);
}

//
// Inserts a row
//
void insertrow(TUIState *tui, int row)
{
    Spreadsheet *sheet = get_spreadsheet();

    spreadsheet_insert_row(sheet, row);
    while (row <= tui->bottomrow)
        displayrow(tui, row++, NOUPDATE);
    recalc(tui);
}

//
// Executes the commands in the spreadsheet menu
//
void smenu(TUIState *tui)
{
    char filename[MAXINPUT + 1];

    filename[0] = 0;
    switch (getcommand(SMENU, SCOMMAND)) {
    case 0:
        checkforsave(tui);
        loadsheet(tui, filename);
        break;
    case 1:
        savesheet(tui);
        break;
    case 2:
        printsheet(tui);
        break;
    case 3:
        checkforsave(tui);
        clearsheet(tui);
        break;
    }
}

//
// Executes the commands in the column menu
//
void cmenu(TUIState *tui)
{
    switch (getcommand(CMENU, CCOMMAND)) {
    case 0:
        insertcol(tui, tui->curcol);
        break;
    case 1:
        deletecol(tui, tui->curcol);
        break;
    case 2:
        setcolwidth(tui, tui->curcol);
        break;
    }
}

//
// Executes the commands in the row menu
//
void rmenu(TUIState *tui)
{
    switch (getcommand(RMENU, RCOMMAND)) {
    case 0:
        insertrow(tui, tui->currow);
        break;
    case 1:
        deleterow(tui, tui->currow);
        break;
    }
}

//
// Executes the commands in the utility menu
//
void umenu(TUIState *tui)
{
    switch (getcommand(UMENU, UCOMMAND)) {
    case 0:
        recalc(tui);
        break;
    case 1: {
        Spreadsheet *sheet = get_spreadsheet();
        changeformdisplay(!spreadsheet_get_formdisplay(sheet));
        displayscreen(tui, UPDATE);
        break;
    }
    }
}

//
// Executes the commands in the main menu
//
void mainmenu(TUIState *tui)
{
    switch (getcommand(MENU, COMMAND)) {
    case 0:
        smenu(tui);
        break;
    case 1:
        formatcells(tui);
        break;
    case 2: {
        Spreadsheet *sheet = get_spreadsheet();
        spreadsheet_delete_cell(sheet, tui->curcol, tui->currow);
        clearoflags(tui, tui->curcol + 1, tui->currow, UPDATE);
        updateoflags(tui, tui->curcol, tui->currow, UPDATE);
        displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
        spreadsheet_set_last_col(sheet);
        spreadsheet_set_last_row(sheet);
        printfreemem();
        if (spreadsheet_get_autocalc(sheet))
            recalc(tui);
        break;
    }
    case 3:
        gotocell(tui);
        break;
    case 4:
        cmenu(tui);
        break;
    case 5:
        rmenu(tui);
        break;
    case 6: {
        Spreadsheet *sheet = get_spreadsheet();
        CELLPTR curcell    = sheet->cell[tui->curcol][tui->currow];
        editcell(tui, curcell);
        break;
    }
    case 7:
        umenu(tui);
        break;
    case 8: {
        Spreadsheet *sheet = get_spreadsheet();
        changeautocalc(tui, !spreadsheet_get_autocalc(sheet));
        break;
    }
    case 9:
        checkforsave(tui);
        tui->stop = true;
        break;
    }
}
