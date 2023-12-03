//
// Created by jacob on 1/28/19.
//
#pragma once
#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/window.h>

#include <string>

class Main_window : public Gtk::Window {
public:
    Main_window();
    virtual ~Main_window();

private:
    std::string x, y;
    char opc;
    const int BUTTON_HEIGHT = 100, BUTTON_WIDTH = 100;

protected:
    void on_one_button_click();
    void on_two_button_click();
    void on_three_button_click();
    void on_four_button_click();
    void on_five_button_click();
    void on_six_button_click();
    void on_seven_button_click();
    void on_eight_button_click();
    void on_nine_button_click();
    void on_zero_button_click();
    void on_plus_button_click();
    void on_minus_button_click();
    void on_multiply_button_click();
    void on_divide_button_click();
    void on_clear_button_click();
    void on_equal_button_click();

    Gtk::Button one_button;
    Gtk::Button two_button;
    Gtk::Button three_button;
    Gtk::Button four_button;
    Gtk::Button five_button;
    Gtk::Button six_button;
    Gtk::Button seven_button;
    Gtk::Button eight_button;
    Gtk::Button nine_button;
    Gtk::Button zero_button;
    Gtk::Button plus_button;
    Gtk::Button minus_button;
    Gtk::Button multiply_button;
    Gtk::Button divide_button;
    Gtk::Button clear_button;
    Gtk::Button equal_buttton;

    Gtk::Entry display;
    Gtk::Grid grid;
};
