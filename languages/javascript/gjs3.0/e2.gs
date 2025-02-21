#!/usr/bin/env gjs

imports.gi.versions.Gtk = '3.0';
const {GObject, Gtk} = imports.gi;

Gtk.init(null);

const MyWindow = GObject.registerClass(class MyWindow extends Gtk.Window {
    _init() {
        super._init({ title: "Hello World" });
        this.button = new Gtk.Button({ label: "Click here" });
        this.button.connect("clicked", MyWindow.onButtonClicked);
        this.add(this.button);
    }

    static onButtonClicked() {
        print("Hello World");
    }
});

let win = new MyWindow();
win.connect("delete-event", () => Gtk.main_quit());
win.show_all();
Gtk.main();
