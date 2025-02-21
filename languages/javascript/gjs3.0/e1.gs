#!/usr/bin/env gjs

imports.gi.versions.Gtk = '3.0';
const Gtk = imports.gi.Gtk;

Gtk.init(null);

let win = new Gtk.Window();
win.connect("delete-event", Gtk.main_quit);
win.show_all();
Gtk.main();
