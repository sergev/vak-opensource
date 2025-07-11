#include "main_window.h"
#include <glibmm/spawn.h>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

MyWindow::MyWindow()
{
    set_title("Sort App");
    set_size_request(400, 600);  // Minimum window size

    set_child(m_box);
    m_box.set_spacing(20);
    m_box.set_margin(10);
    m_box.set_margin_bottom(20);  // Bottom padding

    // CSS for monospaced font and borders
    auto css = Gtk::CssProvider::create();
    css->load_from_data(".mono-text { font-family: monospace; font-size: 14pt; } .bordered { border: 1px solid gray; }");
    Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    m_input_label.set_halign(Gtk::Align::START);
    m_input_label.set_margin_bottom(5);
    m_box.append(m_input_label);

    m_input_scrolled.set_child(m_input_textview);
    m_input_scrolled.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_input_scrolled.set_expand(true);
    m_input_scrolled.add_css_class("bordered");
    m_box.append(m_input_scrolled);

    m_input_textview.set_editable(true);
    m_input_textview.set_wrap_mode(Gtk::WrapMode::WORD);
    m_input_textview.add_css_class("mono-text");

    auto input_buffer = m_input_textview.get_buffer();
    input_buffer->set_text("banana\napple\ncherry\ndate\n");

    m_run_button.set_margin(10);
    m_run_button.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_run_clicked));
    m_box.append(m_run_button);

    m_output_label.set_halign(Gtk::Align::START);
    m_output_label.set_margin_bottom(5);
    m_box.append(m_output_label);

    m_output_scrolled.set_child(m_output_textview);
    m_output_scrolled.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    m_output_scrolled.set_expand(true);
    m_output_scrolled.add_css_class("bordered");
    m_box.append(m_output_scrolled);

    m_output_textview.set_editable(false);
    m_output_textview.set_wrap_mode(Gtk::WrapMode::WORD);
    m_output_textview.add_css_class("mono-text");
}

void MyWindow::on_run_clicked()
{
    auto input_buffer = m_input_textview.get_buffer();
    std::string input_text = input_buffer->get_text();

    std::vector<std::string> argv = {"sort"};

    int child_stdin, child_stdout, child_stderr;
    Glib::Pid pid;

    try {
        Glib::spawn_async_with_pipes(
            "",  // working directory
            argv,
            std::vector<std::string>(),  // envp
            Glib::SpawnFlags::SEARCH_PATH,
            sigc::slot<void()>(),  // no child setup
            &pid,
            &child_stdin,
            &child_stdout,
            &child_stderr
        );

        // Write to child's stdin
        ::write(child_stdin, input_text.c_str(), input_text.length());
        ::close(child_stdin);

        // Read from child's stdout
        std::string output;
        char buf[4096];
        ssize_t n;
        while ((n = ::read(child_stdout, buf, sizeof(buf))) > 0) {
            output.append(buf, n);
        }
        ::close(child_stdout);

        // Read from child's stderr
        std::string error_str;
        while ((n = ::read(child_stderr, buf, sizeof(buf))) > 0) {
            error_str.append(buf, n);
        }
        ::close(child_stderr);

        // Wait for child to exit
        int status;
        ::waitpid(static_cast<pid_t>(pid), &status, 0);
        Glib::spawn_close_pid(pid);

        auto output_buffer = m_output_textview.get_buffer();
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            output_buffer->set_text(output);
        } else {
            output_buffer->set_text("Error: " + error_str + " (exit status: " + std::to_string(WEXITSTATUS(status)) + ")");
        }
    } catch (const Glib::SpawnError& ex) {
        auto output_buffer = m_output_textview.get_buffer();
        output_buffer->set_text("Spawn error: " + std::string(ex.what()));
    }
}
