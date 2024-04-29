#include "editor.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <ncurses.h>
#include <unistd.h>

Editor::Editor()
{
    /* For a new empty file */
    buff.append_line("");
}

void Editor::load_file(std::string fn)
{
    /* Read from file */
    std::ifstream infile(fn.c_str());
    if (!infile.is_open()) {
        std::cerr << "Cannot open file: " << fn << std::endl;
        return;
    }
    filename = fn;

    buff.clear();
    while (!infile.eof()) {
        std::string temp;
        getline(infile, temp);
        buff.append_line(temp);
    }
    infile.close();
}

void Editor::run()
{
    for (;;) {
        if (!status_message_flag) {
            update_status();
        }
        status_message_flag = false;
        print_status_line();
        if (quit_flag) {
            // Pause to show message.
            refresh();
            usleep(500000);
            break;
        }
        print_buff();

        int input = getch();
        if (cmd_flag) {
            handle_command(input);
        } else {
            handle_input(input);
        }
    }
}

void Editor::update_status()
{
    if (cmd_flag) {
        status = "Cmd: " + cmd;
    } else {
        status = "Line=" + std::to_string(y + lowerbound + 1) +
                 "    Col=" + std::to_string(x + 1) +
                 "    \"" + filename +
                 "\" (" + std::to_string((y + lowerbound) * 100 / buff.size()) + "%%)";
    }
}

//
// Process key in Cmd mode.
//
void Editor::handle_command(int c)
{
    switch (c) {
    case KEY_LEFT:
        move_left();
        break;
    case KEY_RIGHT:
        move_right();
        break;
    case KEY_UP:
        move_up();
        break;
    case KEY_DOWN:
        move_down();
        break;
    case KEY_ENTER:
    case 10:
        // Execute the command
        exec_cmd();
        // Switch to input mode
        cmd_flag = false;
        break;
    case 27:
        // Escape/Alt key
        // clears command
        cmd.clear();
        break;
    case 127:
    case KEY_BACKSPACE:
    case KEY_DC:
        // Removes last character
        if (!cmd.empty())
            cmd.erase(cmd.length() - 1, 1);
        break;
    default:
        // Add character to command
        cmd += std::string(1, char(c));
        break;
    }
}

//
// Process key in Input mode.
//
void Editor::handle_input(int c)
{
    switch (c) {
    case 1:
    case KEY_F(1):
        // ^A or F1 key - Switch to command mode
        cmd_flag = true;
        break;
    case KEY_F(2):
        // F2 key - Save file
        save_file();
        break;
    case 127:
    case KEY_BACKSPACE:
        // The Backspace
        if (x == 0 && y > 0) {
            x = buff.line_length(y - 1);
            // Bring line down
            buff.line(y - 1) += buff.line(y);
            // Delete the line
            buff.remove_line(y);
            move_up();
        } else if (x > 0) {
            buff.line(y).erase(--x, 1);
        }
        break;
    case KEY_DC:
        // The Delete key
        if (x == buff.line_length(y) && y != buff.size() - 1) {
            // Bring line down
            buff.line(y) += buff.line(y + 1);
            // Delete the line
            buff.remove_line(y + 1);
        } else {
            buff.line(y).erase(x, 1);
        }
        break;
    case KEY_LEFT:
        move_left();
        break;
    case KEY_RIGHT:
        move_right();
        break;
    case KEY_UP:
        move_up();
        break;
    case KEY_DOWN:
        move_down();
        break;
    case KEY_ENTER:
    case 10:
        // Bring rest of line down
        if (x < buff.line_length(y + lowerbound) - 1) {
            // Put rest of line on new line
            buff.insert_line(
                buff.line(y + lowerbound).substr(x, buff.line_length(y + lowerbound) - x),
                y + 1);
            // Remove that part of the line
            buff.line(y + lowerbound).erase(x, buff.line_length(y + lowerbound) - x);
        } else
            buff.insert_line("", y + lowerbound + 1);
        move_down();
        break;
    case KEY_BTAB:
    case KEY_CTAB:
    case KEY_STAB:
    case KEY_CATAB:
    case 9:
        // The tab
        buff.line(y + lowerbound).insert(x, 4, ' ');
        x += 4;
        break;
    default:
        buff.line(y + lowerbound).insert(x, 1, char(c));
        x++;
        break;
    }
}

void Editor::move_left()
{
    if (x - 1 >= 0) {
        x--;
        move(y, x);
    }
}

void Editor::move_right()
{
    if (x + 1 < COLS && x + 1 <= buff.line_length(y)) {
        x++;
        move(y, x);
    }
}

void Editor::move_up()
{
    if (y - 1 >= 0) {
        y--;
    } else if (y - 1 < 0 && lowerbound > 0) {
        lowerbound--;
    }
    if (x >= buff.line_length(y))
        x = buff.line_length(y);
    move(y, x);
}

void Editor::move_down()
{
    if (y + 1 < LINES - 1 && y + 1 < buff.size()) {
        y++;
    } else if (lowerbound + y < buff.size()) {
        lowerbound++;
    }
    if (x >= buff.line_length(y))
        x = buff.line_length(y);
    move(y, x);
}

void Editor::print_buff()
{
    int lc = 0; // Line count
    for (int i = lowerbound; lc < LINES - 1; i++) {
        if (i < buff.size()) {
            mvprintw(lc, 0, buff.line(i).c_str());
        }
        clrtoeol();
        lc++;
    }
    move(y, x);
}

void Editor::print_status_line()
{
    attron(A_REVERSE);
    mvprintw(LINES - 1, 0, status.c_str());
    clrtoeol();
    attroff(A_REVERSE);
}

void Editor::save_file()
{
    std::ofstream f(filename.c_str());
    if (f.is_open()) {
        for (int i = 0; i < buff.size(); i++) {
            f << buff.line(i) << std::endl;
        }
        status = "Saved to file!";
    } else {
        status = filename + ": Cannot open for writing!";
    }
    status_message_flag = true;
    f.close();
}

void Editor::exec_cmd()
{
    switch (cmd[0]) {
    case 'q':
        // Quit from editor.
        quit_flag = true;
        status = "Exiting";
        status_message_flag = true;
        break;
    case 's':
        // Save buffer to file.
        save_file();
        break;
    }

    // Reset command buffer
    cmd = "";
}
