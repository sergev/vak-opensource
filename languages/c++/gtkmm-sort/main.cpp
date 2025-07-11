#include <gtkmm/application.h>

#include "main_window.h"

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create("com.example.sortapp");

    // Shows the window and returns when it is closed.
    return app->make_window_and_run<MyWindow>(argc, argv);
}
