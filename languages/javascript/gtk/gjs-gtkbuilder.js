//
// Run: gjs-console gjs-gtkbuilder.js
//
const Gtk = imports.gi.Gtk;
Gtk.init (null, null);

let builder = new Gtk.Builder ();
builder.add_from_file ("demo.xml")

let errlog = builder.get_object ("textview_log");

builder.get_object("window_top").connect ("destroy", function () {
	Gtk.main_quit();
});

builder.get_object("button_quit").connect ("clicked", function () {
	Gtk.main_quit();
});

builder.get_object("button_run").connect ("clicked", function () {
	errlog.get_buffer().insert_at_cursor ("Run clicked\n", -1);
});

builder.get_object("button_halt").connect ("clicked", function () {
	errlog.get_buffer().insert_at_cursor ("Halt clicked\n", -1);
});

builder.get_object("button_step").connect ("clicked", function () {
	errlog.get_buffer().insert_at_cursor ("Step clicked\n", -1);
});

builder.get_object("button_reset").connect ("clicked", function () {
	errlog.get_buffer().insert_at_cursor ("Reset clicked\n", -1);
});

Gtk.main ();
