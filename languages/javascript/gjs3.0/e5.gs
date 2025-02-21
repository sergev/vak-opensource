#!/usr/bin/env gjs

imports.gi.versions.Gtk = '3.0';
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

Gtk.init(null);

const ListBoxRowWithData = Lang.Class({
    Name: "ListBoxRowWithData",
    Extends: Gtk.ListBoxRow,

    _init: function(data) {
        this.parent();
        this.data = data;
        this.add(new Gtk.Label({label: data}));
    }
});

const ListBoxWindow = Lang.Class({
    Name: "ListBoxWindow",
    Extends: Gtk.Window,

    _init: function() {
        this.parent({title: "ListBox Demo"});
        this.border_width = 10;

        let box_outer = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 6});
        this.add(box_outer);

        let listbox = new Gtk.ListBox();
        listbox.selection_mode = Gtk.SelectionMode.NONE;
        box_outer.pack_start(listbox, true, true, 0);

        let row = new Gtk.ListBoxRow();
        let hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 50});
        row.add(hbox);
        let vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL});
        hbox.pack_start(vbox, true, true, 0);

        let label1 = new Gtk.Label({label: "Automatic Date & Time", xalign: 0});
        let label2 = new Gtk.Label({label: "Requires internet access", xalign: 0});
        vbox.pack_start(label1, true, true, 0);
        vbox.pack_start(label2, true, true, 0);

        let swtch = new Gtk.Switch();
        swtch.valign = Gtk.Align.CENTER;
        hbox.pack_start(swtch, false, true, 0);

        listbox.add(row);

        row = new Gtk.ListBoxRow();
        hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 50});
        row.add(hbox);
        let label = new Gtk.Label({label: "Enable Automatic Update", xalign: 0});
        let check = new Gtk.CheckButton();
        hbox.pack_start(label, true, true, 0);
        hbox.pack_start(check, false, true, 0);

        listbox.add(row);

        row = new Gtk.ListBoxRow();
        hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 50});
        row.add(hbox);
        label = new Gtk.Label({label: "Date Format", xalign: 0});
        let combo = new Gtk.ComboBoxText();
        combo.insert(0, "0", "24-hour");
        combo.insert(1, "1", "AM/PM");
        hbox.pack_start(label, true, true, 0);
        hbox.pack_start(combo, false, true, 0);

        listbox.add(row);

        let listbox2 = new Gtk.ListBox();
        let items = "This is a sorted ListBox Fail".split(' ');

        items.forEach(
            item => listbox2.add(new ListBoxRowWithData(item))
        );

        let sortFunc = function(row1, row2, data, notifyDestroy) {
            return row1.data.toLowerCase() > row2.data.toLowerCase();
        };

        let filterFunc = function(row, data, notifyDestroy) {
            return (row.data != 'Fail');
        };

        listbox2.set_sort_func(sortFunc, null, false);
        listbox2.set_filter_func(filterFunc, null, false);

        listbox2.connect("row-activated", (widget, row) => print(row.data));

        box_outer.pack_start(listbox2, true, true, 0);
        listbox2.show_all();
    }
});

let win = new ListBoxWindow();
win.connect("delete-event", Gtk.main_quit);
win.show_all();
Gtk.main();
