#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

enum class Key {
    BACKSPACE,
    TAB,
    DELETE,
    DOWN,
    ENTER,
    ESCAPE,
    F1,
    F2,
    LEFT,
    RIGHT,
    UNICODE,
    UP,
};

class Editor_Interface {
public:
    // Get screen dimensions.
    virtual int cols()  = 0;
    virtual int lines() = 0;

    // Get input key and character code.
    virtual Key get_key(unsigned &ch) = 0;

    // Set position of cursor on the screen.
    virtual void set_position(int row, int col) = 0;

    // Print string at given position.
    virtual void print(int row, int col, const std::string &str) = 0;

    // Clear to end of line.
    virtual void clear_to_eol() = 0;

    // Set/clear reverse attribute.
    virtual void set_reverse(bool enable) = 0;
};

class Editor {
private:
    Editor_Interface &ui;
    Buffer buff;
    int x{}, y{};
    int lowerbound{};
    bool cmd_flag{};
    bool quit_flag{};
    bool status_message_flag{};
    std::string filename{ "untitled" };
    std::string status;
    std::string cmd;

public:
    Editor(Editor_Interface &ui_);
    void load_file(std::string);
    void run();

private:
    void handle_command(Key k, unsigned ch);
    void handle_input(Key k, unsigned ch);
    void print_buff();
    void print_status_line();
    void update_status();
    void exec_cmd();
    void move_up();
    void move_down();
    void move_left();
    void move_right();
    void save_file();
};

#endif
