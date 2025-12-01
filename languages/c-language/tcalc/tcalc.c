// Turbo C++ - (C) Copyright 1987-1991 by Borland International
// Converted to ncurses

#define MAIN

#include "tcalc.h"

#include <getopt.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spreadsheet.h"

// Spreadsheet instance - core data structure
static Spreadsheet sheet;

// TUI state (view/cursor position)
static TUIState tui_state = { .stop = false, .nocursor = 0x2000 };

// Get spreadsheet instance (for other modules)
Spreadsheet *get_spreadsheet(void)
{
    return &sheet;
}

// Print help information and exit
static void print_help(const char *progname)
{
    printf("TCALC - Terminal Spreadsheet Application\n");
    printf("\n");
    printf("TCALC is a simple spreadsheet application with a text-based user interface (TUI).\n");
    printf(
        "This version has been converted from the original Borland C++ 2.0 DOS implementation\n");
    printf("to use the ncurses library for Unix/Linux compatibility.\n");
    printf("\n");
    printf("Usage: %s [OPTIONS] [FILE]\n", progname);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help     Display this help message and exit\n");
    printf("  -V, --version  Display version information and exit\n");
    printf("\n");
    printf("Arguments:\n");
    printf("  FILE           Optional spreadsheet file to load on startup\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s                  Start with an empty spreadsheet\n", progname);
    printf("  %s filename.tcalc   Load spreadsheet from file\n", progname);
    printf("\n");
    printf("For more information, see the README file.\n");
}

// Print version information and exit
static void print_version(void)
{
    printf("tcalc - Terminal Spreadsheet Application\n");
    printf("Copyright (C) 1987-1991 Borland International\n");
    printf("Converted to ncurses for Unix/Linux compatibility\n");
    printf("\n");
    printf("Original implementation: Borland C++ 2.0 demonstration program\n");
    printf("This converted version is provided as-is for educational purposes.\n");
}

//
// The main program loop
//
void run(TUIState *tui)
{
    int input;
    Spreadsheet *sheet = get_spreadsheet();

    do {
        displaycell(tui, tui->curcol, tui->currow, HIGHLIGHT, NOUPDATE);
        showcelltype(tui);
        input = getkey();
        switch (input) {
        case '/':
            mainmenu(tui);
            break;
        case F1:
            recalc(tui);
            break;
        case F2: {
            CELLPTR curcell = sheet->cell[tui->curcol][tui->currow];
            editcell(tui, curcell);
            break;
        }
        case DELKEY: {
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
        case PGUPKEY:
            tui->toprow -= 20;
            tui->currow -= 20;
            if (tui->currow < 0)
                tui->currow = tui->toprow = 0;
            else if (tui->toprow < 0) {
                tui->currow -= tui->toprow;
                tui->toprow = 0;
            }
            setbottomrow(tui);
            displayscreen(tui, NOUPDATE);
            break;
        case PGDNKEY:
            tui->toprow += 20;
            tui->currow += 20;
            if ((tui->currow >= MAXROWS) && (tui->toprow >= MAXROWS)) {
                tui->currow = MAXROWS - 1;
                tui->toprow = MAXROWS - 20;
            } else if (tui->toprow > (MAXROWS - 20)) {
                tui->currow -= (tui->toprow + 20 - MAXROWS);
                tui->toprow = MAXROWS - 20;
            }
            setbottomrow(tui);
            displayscreen(tui, NOUPDATE);
            break;
        case CTRLLEFTKEY:
            displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
            if (tui->leftcol == 0)
                tui->curcol = 0;
            else {
                tui->curcol = tui->rightcol = tui->leftcol - 1;
                setleftcol(tui);
                setrightcol(tui);
                displayscreen(tui, NOUPDATE);
            }
            break;
        case CTRLRIGHTKEY:
            displaycell(tui, tui->curcol, tui->currow, NOHIGHLIGHT, NOUPDATE);
            if (tui->rightcol == MAXCOLS - 1)
                tui->curcol = tui->rightcol;
            else {
                tui->curcol = tui->leftcol = tui->rightcol + 1;
                setrightcol(tui);
                setleftcol(tui);
                displayscreen(tui, NOUPDATE);
            }
            break;
        case HOMEKEY:
            tui->currow = tui->curcol = tui->leftcol = tui->toprow = 0;
            setrightcol(tui);
            setbottomrow(tui);
            displayscreen(tui, NOUPDATE);
            break;
        case ENDKEY: {
            tui->rightcol = tui->curcol = sheet->lastcol;
            tui->currow = tui->bottomrow = sheet->lastrow;
            settoprow(tui);
            setleftcol(tui);
            setrightcol(tui);
            displayscreen(tui, NOUPDATE);
            break;
        }
        case UPKEY:
            moverowup(tui);
            break;
        case DOWNKEY:
            moverowdown(tui);
            break;
        case LEFTKEY:
            movecolleft(tui);
            break;
        case RIGHTKEY:
            movecolright(tui);
            break;
        default:
            if ((input >= ' ') && (input <= '~'))
                getinput(tui, input);
            break;
        }
    } while (!tui->stop);
}

int main(int argc, char *argv[])
{
    // Define long options
    static struct option long_options[] = {
        { "help", no_argument, 0, 'h' },    //
        { "version", no_argument, 0, 'V' }, //
        { 0 }                               //
    };

    // Parse command line options
    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hV", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'h':
            print_help(argv[0]);
            return 0;
        case 'V':
            print_version();
            return 0;
        default:
            // getopt_long will print an error message
            print_help(argv[0]);
            return 1;
        }
    }

    // Get filename from remaining positional arguments
    char *filename = NULL;
    if (optind < argc) {
        filename = argv[optind];
    }

    // Initialize ncurses
    initscr();
    start_color();
    keypad(stdscr, true);
    noecho();
    cbreak();

    initcursor(&tui_state);
    initcolortable();
    setcursor(tui_state.nocursor);
    setcolor(LOWCOMMANDCOLOR);
    clear();
    refresh();
    writef((80 - strlen(MSGHEADER)) >> 1, 11, MSGHEADERCOLOR, strlen(MSGHEADER), MSGHEADER);
    writef((80 - strlen(MSGKEYPRESS)) >> 1, 13, PROMPTCOLOR, strlen(MSGKEYPRESS), MSGKEYPRESS);
    move(24, 79); // Row 25 in 1-based = row 24 in 0-based
    refresh();
    getkey();
    setcolor(LOWCOMMANDCOLOR);
    clear();
    refresh();

    // Initialize spreadsheet
    spreadsheet_init(&sheet);

    redrawscreen(&tui_state);
    if (filename != NULL)
        loadsheet(&tui_state, filename);
    clearinput();
    run(&tui_state);
    setcolor(LOWCOMMANDCOLOR);
    clear();
    refresh();
    setcursor(tui_state.oldcursor);
    endwin();
    return 0;
}
