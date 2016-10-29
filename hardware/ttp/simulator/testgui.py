#!/usr/bin/env python
# -*- coding: UTF8 -*-
#
# Проверка внешнего вида интерфейса эмулятора.
#
import gtk
import gtk.glade

def main_quit (widget):
	gtk.main_quit ()

def on_button_run_clicked (widget):
	log.get_buffer().insert_at_cursor ("Нажата кнопка \"Пуск\"\n")

def on_button_halt_clicked (widget):
	log.get_buffer().insert_at_cursor ("Нажата кнопка \"Стоп\"\n")

def on_button_step_clicked (widget):
	log.get_buffer().insert_at_cursor ("Нажата кнопка \"Шаг\"\n")

def on_button_reset_clicked (widget):
	log.get_buffer().insert_at_cursor ("Нажата кнопка \"Сброс\"\n")

xml = gtk.glade.XML ("simulator.glade")
log = xml.get_widget ("textview_log")

xml.get_widget("window_top").connect ("destroy", main_quit)
xml.get_widget("button_quit").connect ("clicked", main_quit)
xml.get_widget("button_run").connect ("clicked", on_button_run_clicked)
xml.get_widget("button_halt").connect ("clicked", on_button_halt_clicked)
xml.get_widget("button_step").connect ("clicked", on_button_step_clicked)
xml.get_widget("button_reset").connect ("clicked", on_button_reset_clicked)

gtk.main()
