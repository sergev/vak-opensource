#ifndef GTKMM_EXAMPLEASSISTANT_H
#define GTKMM_EXAMPLEASSISTANT_H

#include <gtkmm.h>

class ExampleAssistant : public Gtk::Assistant {
public:
    ExampleAssistant();
    virtual ~ExampleAssistant();

    void get_result(bool &check_state, Glib::ustring &entry_text);

private:
    // Signal handlers:
    void on_assistant_apply();
    void on_assistant_cancel();
    void on_assistant_close();
    void on_assistant_prepare(Gtk::Widget *widget);
    void on_entry_changed();

    // Member functions:
    void print_status();

    // Child widgets:
    Gtk::Box m_box;
    Gtk::Label m_label1, m_label2;
    Gtk::CheckButton m_check;
    Gtk::Entry m_entry;
};

#endif /* GTKMM_EXAMPLEASSISTANT_H */
