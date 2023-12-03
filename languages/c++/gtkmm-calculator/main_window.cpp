//
// Created by jacob on 1/29/19.
//
#include "main_window.h"

#include <stdlib.h>

#include <iostream>

Main_window::Main_window()
    : one_button("1"), two_button("2"), three_button("3"), four_button("4"), five_button("5"),
      six_button("6"), seven_button("7"), eight_button("8"), nine_button("9"), zero_button("0"),
      plus_button("+"), minus_button("-"), multiply_button("*"), divide_button("/"),
      clear_button("C"), equal_buttton("=")
{
    // window
    // sets window title
    set_title("Calculator");
    // sets window size
    set_default_size(400, 400);
    set_resizable(false);
    // sets window position on launch
    // set_position(Gtk::WIN_POS_CENTER);
    // textbox
    // sets textbox size
    textbox.set_size_request(400, 50);
    // button
    // creates button labels
    one_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    two_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    three_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    four_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    five_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    six_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    seven_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    eight_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    nine_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    zero_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    plus_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    minus_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    multiply_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    divide_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    clear_button.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    equal_buttton.set_size_request(BUTTON_WIDTH, BUTTON_HEIGHT);
    // click event
    one_button.signal_clicked().connect(sigc::mem_fun(*this, &Main_window::on_one_button_click));
    two_button.signal_clicked().connect(sigc::mem_fun(*this, &Main_window::on_two_button_click));
    three_button.signal_clicked().connect(
        sigc::mem_fun(*this, &Main_window::on_three_button_click));
    four_button.signal_clicked().connect(sigc::mem_fun(*this, &Main_window::on_four_button_click));
    five_button.signal_clicked().connect(sigc::mem_fun(*this, &Main_window::on_five_button_click));
    six_button.signal_clicked().connect(sigc::mem_fun(*this, &Main_window::on_six_button_click));
    seven_button.signal_clicked().connect(
        sigc::mem_fun(*this, &Main_window::on_seven_button_click));
    eight_button.signal_clicked().connect(
        sigc::mem_fun(*this, &Main_window::on_eight_button_click));
    nine_button.signal_clicked().connect(sigc::mem_fun(*this, &Main_window::on_nine_button_click));
    zero_button.signal_clicked().connect(sigc::mem_fun(*this, &Main_window::on_zero_button_click));
    plus_button.signal_clicked().connect(sigc::mem_fun(*this, &Main_window::on_plus_button_click));
    minus_button.signal_clicked().connect(
        sigc::mem_fun(*this, &Main_window::on_minus_button_click));
    multiply_button.signal_clicked().connect(
        sigc::mem_fun(*this, &Main_window::on_multiply_button_click));
    divide_button.signal_clicked().connect(
        sigc::mem_fun(*this, &Main_window::on_divide_button_click));
    clear_button.signal_clicked().connect(
        sigc::mem_fun(*this, &Main_window::on_clear_button_click));
    equal_buttton.signal_clicked().connect(
        sigc::mem_fun(*this, &Main_window::on_equal_button_click));

    // attaches the buttons to the grid
    grid.attach(textbox, 0, 0);

    grid.attach(one_button,   1, 0);
    grid.attach(two_button,   1, 1);
    grid.attach(three_button, 1, 2);
    grid.attach(plus_button,  1, 3);

    grid.attach(four_button,  2, 0);
    grid.attach(five_button,  2, 1);
    grid.attach(six_button,   2, 2);
    grid.attach(minus_button, 2, 3);

    grid.attach(seven_button,    3, 0);
    grid.attach(eight_button,    3, 1);
    grid.attach(nine_button,     3, 2);
    grid.attach(multiply_button, 3, 3);

    grid.attach(clear_button,  4, 0);
    grid.attach(zero_button,   4, 1);
    grid.attach(equal_buttton, 4, 2);
    grid.attach(divide_button, 4, 3);

    //append(grid);
    //show_all_children();
}

Main_window::~Main_window()
{
}
void Main_window::on_one_button_click()
{
    x += "1";
    textbox.set_text(x);
}

void Main_window::on_two_button_click()
{
    x += "2";
    textbox.set_text(x);
}

void Main_window::on_three_button_click()
{
    x += "3";
    textbox.set_text(x);
}

void Main_window::on_four_button_click()
{
    x += "4";
    textbox.set_text(x);
}

void Main_window::on_five_button_click()
{
    x += "5";
    textbox.set_text(x);
}

void Main_window::on_six_button_click()
{
    x += "6";
    textbox.set_text(x);
}

void Main_window::on_seven_button_click()
{
    x += "7";
    textbox.set_text(x);
}

void Main_window::on_eight_button_click()
{
    x += "8";
    textbox.set_text(x);
}

void Main_window::on_nine_button_click()
{
    x += "9";
    textbox.set_text(x);
}

void Main_window::on_zero_button_click()
{
    x += "0";
    textbox.set_text(x);
}

void Main_window::on_plus_button_click()
{
    op = '+';
    opc = '+';
    textbox.set_text(op);
    y = x;
    x = "";
}

void Main_window::on_minus_button_click()
{
    op = '-';
    opc = '-';
    textbox.set_text(op);
    y = x;
    x = "";
}

void Main_window::on_multiply_button_click()
{
    op = '*';
    opc = '*';
    textbox.set_text(op);
    y = x;
    x = "";
}

void Main_window::on_divide_button_click()
{
    op = '/';
    opc = '/';
    textbox.set_text(op);
    y = x;
    x = "";
}

void Main_window::on_clear_button_click()
{
    x = "", y = "";
    op = ' ';
    textbox.set_text("");
}

void Main_window::on_equal_button_click()
{
    switch (opc) {
    case '+':
        strx = strtof(x.c_str(), NULL);
        stry = std::strtof(y.c_str(), NULL);
        z = stry + strx;
        textbox.set_text(std::to_string(z));
        x = std::to_string(z);
        break;
    case '-':
        strx = strtof(x.c_str(), NULL);
        stry = std::strtof(y.c_str(), NULL);
        z = stry - strx;
        textbox.set_text(std::to_string(z));
        x = std::to_string(z);
        break;
    case '*':
        strx = strtof(x.c_str(), NULL);
        stry = std::strtof(y.c_str(), NULL);
        z = stry * strx;
        textbox.set_text(std::to_string(z));
        x = std::to_string(z);
        break;
    case '/':
        strx = strtof(x.c_str(), NULL);
        stry = std::strtof(y.c_str(), NULL);
        z = stry / strx;
        textbox.set_text(std::to_string(z));
        x = std::to_string(z);
        break;
    default:
        textbox.set_text("ERROR");
        break;
    }
}
