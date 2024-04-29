#include <ncurses.h>

#include "editor.h"

int main(int argc, char *argv[])
{
    Editor ed;

    if (argc > 1) {
        // Initialize filename
        std::string filename(argv[1]);
        ed.load_file(filename);
    }

    // Initialize curses
    initscr();
    noecho();
    cbreak(); // Disable line buffering
    keypad(stdscr, true);

    ed.run();

    // Disable curses.
    refresh();
    endwin();
    return 0;
}
