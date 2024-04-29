#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

class Editor {
private:
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
    Editor();
    void load_file(std::string);
    void run();

private:
    void handle_command(int);
    void handle_input(int);
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
