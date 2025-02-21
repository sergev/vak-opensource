#!/usr/bin/env gjs

imports.gi.versions.Gtk = '3.0';
const Gtk = imports.gi.Gtk;
const Gio = imports.gi.Gio;
const Lang = imports.lang;

Gtk.init(null);

const HeaderBarWindow = Lang.Class({
    Name: "HeaderBarWindow",
    Extends: Gtk.Window,

    _init: function() {
        this.parent({title: "HeaderBar Demo"});
        this.set_border_width(10);
        this.set_default_size(400, 200);

        let hb = new Gtk.HeaderBar();
        hb.set_show_close_button(true);
        hb.set_title("HeaderBar Example");
        this.set_titlebar(hb);

        let button = new Gtk.Button();
        let icon = new Gio.ThemedIcon({name: "mail-send-receive-symbolic"});
        let image = Gtk.Image.new_from_gicon(icon, Gtk.IconSize.BUTTON);
        button.add(image);
        hb.pack_end(button);

        let box = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL});

        button = new Gtk.Button();
        button.add(new Gtk.Arrow({arrow_type: Gtk.ArrowType.LEFT, shadow_type: Gtk.ShadowType.NONE}));
        box.add(button);

        button = new Gtk.Button();
        button.add(new Gtk.Arrow({arrow_type: Gtk.ArrowType.RIGHT, shadow_type: Gtk.ShadowType.NONE}));
        box.add(button);

        hb.pack_start(box);

        this.add(new Gtk.TextView());
    }
});

let win = new HeaderBarWindow();
win.connect("delete-event", Gtk.main_quit);
win.show_all();
Gtk.main();
