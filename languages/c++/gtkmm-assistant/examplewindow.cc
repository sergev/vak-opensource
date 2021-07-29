#include "examplewindow.h"

#include "exampleassistant.h"

ExampleWindow::ExampleWindow()
    : m_table(3, 2), m_button("Show the assistant"), m_label1("State of assistant checkbutton:"),
      m_label2("Contents of assistant entry:")
{
    set_title("Gtk::Assistant example");
    set_border_width(12);

    m_table.attach(m_button, 0, 2, 0, 1, Gtk::FILL, Gtk::EXPAND);
    m_table.attach(m_label1, 0, 1, 1, 2, Gtk::FILL, Gtk::EXPAND);
    m_table.attach(m_label2, 0, 1, 2, 3, Gtk::FILL, Gtk::EXPAND);
    m_table.attach(m_check, 1, 2, 1, 2);
    m_table.attach(m_entry, 1, 2, 2, 3);
    add(m_table);

    m_label1.set_alignment(0.0, 0.5);
    m_label2.set_alignment(0.0, 0.5);

    m_button.signal_clicked().connect(sigc::mem_fun(*this, &ExampleWindow::on_button_clicked));
    m_assistant.signal_apply().connect(sigc::mem_fun(*this, &ExampleWindow::on_assistant_apply));

    m_check.set_sensitive(false);
    m_entry.set_sensitive(false);

    show_all_children();
}

ExampleWindow::~ExampleWindow()
{
}

void ExampleWindow::on_assistant_apply()
{
    bool check_state;
    Glib::ustring entry_text;

    m_assistant.get_result(check_state, entry_text);
    m_check.set_active(check_state);
    m_entry.set_text(entry_text);
}

void ExampleWindow::on_button_clicked()
{
    m_assistant.show();
}
