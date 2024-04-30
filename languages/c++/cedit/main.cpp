#include <ncurses.h>
#include <unistd.h>

#include "editor.h"

class Curses_Interface : public Editor_Interface {
public:
    int cols() override;
    int lines() override;
    Key get_key(unsigned &ch) override;
    void set_position(int row, int col) override;
    void print(int row, int col, const std::string &str) override;
    void clear_to_eol() override;
    void set_reverse(bool enable) override;
};

//
// Get input key and character code.
//
Key Curses_Interface::get_key(unsigned &ch)
{
    ch = getch();

    switch (ch) {
    case KEY_LEFT:
        return Key::LEFT;
    case KEY_RIGHT:
        return Key::RIGHT;
    case KEY_UP:
        return Key::UP;
    case KEY_DOWN:
        return Key::DOWN;
    case KEY_ENTER:
        return Key::ENTER;
    case '\n':
        return Key::ENTER;
    case '\033':
        return Key::ESCAPE;
    case '\177':
        return Key::BACKSPACE;
    case KEY_BACKSPACE:
        return Key::BACKSPACE;
    case KEY_DC:
        return Key::DELETE;
    case '\001':
        return Key::F1; // ^A
    case KEY_F(1):
        return Key::F1;
    case KEY_F(2):
        return Key::F2;
    case KEY_BTAB:
        return Key::TAB;
    case KEY_CTAB:
        return Key::TAB;
    case KEY_STAB:
        return Key::TAB;
    case KEY_CATAB:
        return Key::TAB;
    case '\t':
        return Key::TAB;
    default:
        return Key::UNICODE;
    }
}

//
// Set position of cursor on the screen.
//
void Curses_Interface::set_position(int row, int col)
{
    move(row, col);
}

int Curses_Interface::cols()
{
    return COLS;
}

int Curses_Interface::lines()
{
    return LINES;
}

//
// Print string at given position.
//
void Curses_Interface::print(int row, int col, const std::string &str)
{
    mvprintw(row, col, str.c_str());
}

//
// Clear to end of line.
//
void Curses_Interface::clear_to_eol()
{
    clrtoeol();
}

//
// Set/clear reverse attribute.
//
void Curses_Interface::set_reverse(bool enable)
{
    if (enable) {
        attron(A_REVERSE);
    } else {
        attroff(A_REVERSE);
    }
}

int main(int argc, char *argv[])
{
    Curses_Interface ui;
    Editor editor(ui);

    if (argc > 1) {
        // Initialize filename
        std::string filename(argv[1]);
        editor.load_file(filename);
    }

    // Initialize curses
    initscr();
    noecho();
    cbreak(); // Disable line buffering
    keypad(stdscr, true);

    editor.run();

    // Disable curses.
    refresh();
    usleep(500000); // Pause to show last message
    endwin();
    return 0;
}
