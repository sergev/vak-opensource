//
// Run: gjs-console gjs-hello.js
//
const Gtk = imports.gi.Gtk;

Gtk.init (null, null);

let win = new Gtk.Window({ type: Gtk.WindowType.TOPLEVEL });
win.set_border_width(10);
win.connect("destroy", function () { Gtk.main_quit(); });

let button = new Gtk.Button({ label: "Hello World" });
button.connect("clicked", function() { win.destroy(); });

win.add(button);
win.show_all();
Gtk.main();
