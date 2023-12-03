/* gtkmm example Copyright (C) 2002 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include "helloworld.h"

#include <iostream>

HelloWorld::HelloWorld() : m_button1("Button 1"), m_button2("Button 2")
{
    // This just sets the title of our new window.
    set_title("Hello Buttons!");

    // Sets the margin around the box.
    m_box1.set_margin(10);

    // put the box into the main window.
    set_child(m_box1);

    // Now when the button is clicked, we call the on_button_clicked() function
    // with a pointer to "button 1" as its argument.
    m_button1.signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this, &HelloWorld::on_button_clicked), "button 1"));

    // We use Gtk::Box::append() to pack this button into the box,
    // which has been packed into the window.
    // A widget's default behaviour is not to expand if extra space is available.
    // A container widget by default expands if any of the contained widgets
    // wants to expand.
    m_box1.append(m_button1);
    m_button1.set_expand();

    // call the same signal handler with a different argument,
    // passing a pointer to "button 2" instead.
    m_button2.signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this, &HelloWorld::on_button_clicked), "button 2"));

    m_box1.append(m_button2);
    m_button2.set_expand();

    // Gtk::Widget::set_visible(true) is seldom needed. All widgets are visible by default.
}

HelloWorld::~HelloWorld()
{
}

// Our new improved signal handler.  The data passed to this method is
// printed to stdout.
void HelloWorld::on_button_clicked(const Glib::ustring &data)
{
    std::cout << "Hello World - " << data << " was pressed" << std::endl;
}
