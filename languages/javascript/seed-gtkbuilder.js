#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

var builder = new Gtk.Builder ();
builder.add_from_file ("demo.xml")

var errlog = builder.get_object ("textview_log");

builder.get_object("window_top").signal.hide.connect (function () {
	Gtk.main_quit();
});

builder.get_object("button_quit").signal.clicked.connect (function () {
	Gtk.main_quit();
});

builder.get_object("button_run").signal.clicked.connect (function () {
	errlog.get_buffer().insert_at_cursor ("Run clicked\n", -1);
});

builder.get_object("button_halt").signal.clicked.connect (function () {
	errlog.get_buffer().insert_at_cursor ("Halt clicked\n", -1);
});

builder.get_object("button_step").signal.clicked.connect (function () {
	errlog.get_buffer().insert_at_cursor ("Step clicked\n", -1);
});

builder.get_object("button_reset").signal.clicked.connect (function () {
	errlog.get_buffer().insert_at_cursor ("Reset clicked\n", -1);
});

Gtk.main ();
