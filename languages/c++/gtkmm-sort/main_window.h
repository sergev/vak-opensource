#include <gtkmm.h>
#include <string>

class MyWindow : public Gtk::Window {
public:
    MyWindow();

private:
    Gtk::Box m_box{Gtk::Orientation::VERTICAL};
    Gtk::Label m_input_label{"Input (Editable):"};
    Gtk::ScrolledWindow m_input_scrolled;
    Gtk::TextView m_input_textview;
    Gtk::Button m_run_button{"Run"};
    Gtk::Label m_output_label{"Output (Read-only):"};
    Gtk::ScrolledWindow m_output_scrolled;
    Gtk::TextView m_output_textview;

    void on_run_clicked();
};
