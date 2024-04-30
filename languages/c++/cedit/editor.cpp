#include "editor.h"

#include <fstream>
#include <iostream>

Editor::Editor(Editor_Interface &ui_) : ui(ui_)
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
            break;
        }
        print_buff();

        unsigned ch{};
        Key k = ui.get_key(ch);
        if (cmd_flag) {
            handle_command(k, ch);
        } else {
            handle_input(k, ch);
        }
    }
}

void Editor::update_status()
{
    if (cmd_flag) {
        status = "Cmd: " + cmd;
    } else {
        status = "Line=" + std::to_string(y + lowerbound + 1) + "    Col=" + std::to_string(x + 1) +
                 "    \"" + filename + "\" (" +
                 std::to_string((y + lowerbound) * 100 / buff.size()) + "%%)";
    }
}

//
// Process key in Cmd mode.
//
void Editor::handle_command(Key k, unsigned ch)
{
    switch (k) {
    case Key::LEFT:
        move_left();
        break;
    case Key::RIGHT:
        move_right();
        break;
    case Key::UP:
        move_up();
        break;
    case Key::DOWN:
        move_down();
        break;
    case Key::ENTER:
        // Execute the command
        exec_cmd();
        // Switch to input mode
        cmd_flag = false;
        break;
    case Key::ESCAPE:
        // Escape/Alt key
        // clears command
        cmd.clear();
        break;
    case Key::BACKSPACE:
    case Key::DELETE:
        // Removes last character
        if (!cmd.empty())
            cmd.erase(cmd.length() - 1, 1);
        break;
    case Key::UNICODE:
        // Add character to command
        cmd += std::string(1, char(ch));
        break;
    case Key::TAB:
    case Key::F1:
    case Key::F2:
        // Ignore.
        break;
    }
}

//
// Process key in Input mode.
//
void Editor::handle_input(Key k, unsigned ch)
{
    switch (k) {
    case Key::F1:
        // ^A or F1 key - Switch to command mode
        cmd_flag = true;
        break;
    case Key::F2:
        // F2 key - Save file
        save_file();
        break;
    case Key::BACKSPACE:
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
    case Key::DELETE:
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
    case Key::LEFT:
        move_left();
        break;
    case Key::RIGHT:
        move_right();
        break;
    case Key::UP:
        move_up();
        break;
    case Key::DOWN:
        move_down();
        break;
    case Key::ENTER:
        // Bring rest of line down
        if (x < buff.line_length(y + lowerbound) - 1) {
            // Put rest of line on new line
            buff.insert_line(
                buff.line(y + lowerbound).substr(x, buff.line_length(y + lowerbound) - x), y + 1);
            // Remove that part of the line
            buff.line(y + lowerbound).erase(x, buff.line_length(y + lowerbound) - x);
        } else
            buff.insert_line("", y + lowerbound + 1);
        move_down();
        break;
    case Key::TAB:
        // The tab
        buff.line(y + lowerbound).insert(x, 4, ' ');
        x += 4;
        break;
    case Key::UNICODE:
        buff.line(y + lowerbound).insert(x, 1, char(ch));
        x++;
        break;
    case Key::ESCAPE:
        // Ignore.
        break;
    }
}

void Editor::move_left()
{
    if (x - 1 >= 0) {
        x--;
        ui.set_position(y, x);
    }
}

void Editor::move_right()
{
    if (x + 1 < ui.cols() && x + 1 <= buff.line_length(y)) {
        x++;
        ui.set_position(y, x);
    }
}

void Editor::move_up()
{
    if (lowerbound + y < 1) {
        // No lines before the current one.
        return;
    }
    if (y > 0) {
        y--;
    } else {
        lowerbound--;
    }
    if (x >= buff.line_length(y))
        x = buff.line_length(y);
    ui.set_position(y, x);
}

void Editor::move_down()
{
    if (lowerbound + y + 1 >= buff.size()) {
        // No more lines in the file.
        return;
    }
    if (y + 1 < ui.lines() - 1) { // Next line fits into the screen
        y++;
    } else {
        lowerbound++;
    }
    if (x >= buff.line_length(y))
        x = buff.line_length(y);
    ui.set_position(y, x);
}

void Editor::print_buff()
{
    int lc = 0; // Line count
    for (int i = lowerbound; lc < ui.lines() - 1; i++) {
        if (i < buff.size()) {
            ui.print(lc, 0, buff.line(i));
        } else {
            ui.print(lc, 0, "~");
        }
        ui.clear_to_eol();
        lc++;
    }
    ui.set_position(y, x);
}

void Editor::print_status_line()
{
    ui.set_reverse(true);
    ui.print(ui.lines() - 1, 0, status);
    ui.clear_to_eol();
    ui.set_reverse(false);
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
        quit_flag           = true;
        status              = "Exiting";
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
