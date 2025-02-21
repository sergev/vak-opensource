#!/usr/bin/env gjs

imports.gi.versions.Gtk = '3.0';
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
const Lang = imports.lang;

Gtk.init(null);

const FlowBoxWindow = Lang.Class({
    Name: "FlowBoxWindow",
    Extends: Gtk.Window,

    _init: function() {
        this.parent({title: "FlowBox Demo"});
        this.set_border_width(10);
        this.set_default_size(300, 250);

        let header = new Gtk.HeaderBar({title: "Flow Box"});
        header.set_subtitle("Sample FlowBox App");
        header.set_show_close_button(true);

        this.set_titlebar(header);

        let scrolled = new Gtk.ScrolledWindow();
        scrolled.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC);

        let flowbox = new Gtk.FlowBox();
        flowbox.set_valign(Gtk.Align.START);
        flowbox.set_max_children_per_line(30);
        flowbox.set_selection_mode(Gtk.SelectionMode.NONE);

        this.createFlowbox(flowbox);

        scrolled.add(flowbox);

        this.add(scrolled);
    },

    colorSwatchNew: function(strColor) {
        let rgba = new Gdk.RGBA();
        rgba.parse(strColor);

        let button = Gtk.ColorButton.new_with_rgba(rgba);

        return button;
    },

    createFlowbox: function(flowbox) {
        let colors = [
        'AliceBlue',
        'AntiqueWhite',
        'AntiqueWhite1',
        'AntiqueWhite2',
        'AntiqueWhite3',
        'AntiqueWhite4',
        'aqua',
        'aquamarine',
        'aquamarine1',
        'aquamarine2',
        'aquamarine3',
        'aquamarine4',
        'azure',
        'azure1',
        'azure2',
        'azure3',
        'azure4',
        'beige',
        'bisque',
        'bisque1',
        'bisque2',
        'bisque3',
        'bisque4',
        'black',
        'BlanchedAlmond',
        'blue',
        'blue1',
        'blue2',
        'blue3',
        'blue4',
        'BlueViolet',
        'brown',
        'brown1',
        'brown2',
        'brown3',
        'brown4',
        'burlywood',
        'burlywood1',
        'burlywood2',
        'burlywood3',
        'burlywood4',
        'CadetBlue',
        'CadetBlue1',
        'CadetBlue2',
        'CadetBlue3',
        'CadetBlue4',
        'chartreuse',
        'chartreuse1',
        'chartreuse2',
        'chartreuse3',
        'chartreuse4',
        'chocolate',
        'chocolate1',
        'chocolate2',
        'chocolate3',
        'chocolate4',
        'coral',
        'coral1',
        'coral2',
        'coral3',
        'coral4'
        ];

        colors.forEach(
            color => flowbox.add(this.colorSwatchNew(color))
        );
    }
});

let win = new FlowBoxWindow();
win.connect("delete-event", Gtk.main_quit);
win.show_all();
Gtk.main();
