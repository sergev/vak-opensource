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
    set_title("Calculator");
    set_default_size(400, 400);
    set_resizable(false);

    display.set_size_request(400, 50);

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
    grid.attach(display, 0, 0, 4);

    grid.attach(one_button,   0, 1);
    grid.attach(two_button,   1, 1);
    grid.attach(three_button, 2, 1);
    grid.attach(plus_button,  3, 1);

    grid.attach(four_button,  0, 2);
    grid.attach(five_button,  1, 2);
    grid.attach(six_button,   2, 2);
    grid.attach(minus_button, 3, 2);

    grid.attach(seven_button,    0, 3);
    grid.attach(eight_button,    1, 3);
    grid.attach(nine_button,     2, 3);
    grid.attach(multiply_button, 3, 3);

    grid.attach(clear_button,  0, 4);
    grid.attach(zero_button,   1, 4);
    grid.attach(equal_buttton, 2, 4);
    grid.attach(divide_button, 3, 4);

    set_child(grid);
}

Main_window::~Main_window()
{
}
void Main_window::on_one_button_click()
{
    x += "1";
    display.set_text(x);
}

void Main_window::on_two_button_click()
{
    x += "2";
    display.set_text(x);
}

void Main_window::on_three_button_click()
{
    x += "3";
    display.set_text(x);
}

void Main_window::on_four_button_click()
{
    x += "4";
    display.set_text(x);
}

void Main_window::on_five_button_click()
{
    x += "5";
    display.set_text(x);
}

void Main_window::on_six_button_click()
{
    x += "6";
    display.set_text(x);
}

void Main_window::on_seven_button_click()
{
    x += "7";
    display.set_text(x);
}

void Main_window::on_eight_button_click()
{
    x += "8";
    display.set_text(x);
}

void Main_window::on_nine_button_click()
{
    x += "9";
    display.set_text(x);
}

void Main_window::on_zero_button_click()
{
    x += "0";
    display.set_text(x);
}

void Main_window::on_plus_button_click()
{
    opc = '+';
    display.set_text("+");
    y = x;
    x = "";
}

void Main_window::on_minus_button_click()
{
    opc = '-';
    display.set_text("-");
    y = x;
    x = "";
}

void Main_window::on_multiply_button_click()
{
    opc = '*';
    display.set_text("*");
    y = x;
    x = "";
}

void Main_window::on_divide_button_click()
{
    opc = '/';
    display.set_text("/");
    y = x;
    x = "";
}

void Main_window::on_clear_button_click()
{
    x = "";
    y = "";
    display.set_text("");
}

void Main_window::on_equal_button_click()
{
    float a = std::stof(x);
    float b = std::stof(y);
    float z;

    switch (opc) {
    case '+':
        z = b + a;
        break;
    case '-':
        z = b - a;
        break;
    case '*':
        z = b * a;
        break;
    case '/':
        z = b / a;
        break;
    default:
        throw std::runtime_error("Invalid operation");
    }
    x = std::to_string(z);
    display.set_text(x);
}
