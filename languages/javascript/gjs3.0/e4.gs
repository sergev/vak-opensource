#!/usr/bin/env gjs

imports.gi.versions.Gtk = '3.0';
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

Gtk.init(null);

const GridWindow = Lang.Class({
    Name: 'GridWindow',
    Extends: Gtk.Window,

    _init: function() {
        this.parent({title: "Grid Example"});

        let grid = new Gtk.Grid();
        this.add(grid);

        let button1 = new Gtk.Button({label: "Button 1"});
        let button2 = new Gtk.Button({label: "Button 2"});
        let button3 = new Gtk.Button({label: "Button 3"});
        let button4 = new Gtk.Button({label: "Button 4"});
        let button5 = new Gtk.Button({label: "Button 5"});
        let button6 = new Gtk.Button({label: "Button 6"});

        grid.add(button1);
        grid.attach(button2, 1, 0, 2, 1);
        grid.attach_next_to(button3, button1, Gtk.PositionType.BOTTOM, 1, 2);
        grid.attach_next_to(button4, button3, Gtk.PositionType.RIGHT, 2, 1);
        grid.attach(button5, 1, 2, 1, 1);
        grid.attach_next_to(button6, button5, Gtk.PositionType.RIGHT, 1, 1);
    }
});

let win = new GridWindow();
win.connect("delete-event", Gtk.main_quit);
win.show_all();
Gtk.main();
