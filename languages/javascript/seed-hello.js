#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

var win = new Gtk.Window();
win.set_border_width(10);
win.signal.hide.connect(Gtk.main_quit);

var button = new Gtk.Button({ label: "Hello World" });
button.signal.clicked.connect(function() { win.destroy(); });

win.add(button);
win.show_all();
Gtk.main();
