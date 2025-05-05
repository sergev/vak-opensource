#include <ncurses.h>
#include <locale.h>
#include <iostream>

int main() {
    // Setting up locale for wide character support
    setlocale(LC_ALL, "");

    // Initialize curses
    initscr();
    cbreak();
    noecho();
    nonl();
    keypad(stdscr, TRUE);

    // Print instructions
    printw("Press any key to see its wide character code (ESC to exit):\n");

    // Main loop to capture and display wide character codes
    wint_t ch;
    while (true) {
        // Get wide character input
        if (get_wch(&ch) != ERR) {
            // Check for ESC key (27)
            if (ch == 27) {
                break;
            }

            // Clear screen and print the character code
            clear();
            printw("Pressed key code: %u (0x%X)\n", ch, ch);
            printw("Character: %lc\n", ch);
            printw("\nPress any key to continue (ESC to exit):\n");
            refresh();
        }
    }

    // Clean up and exit
    endwin();
    return 0;
}
