all:		demo.xml

edit:		demo.glade
		glade-2 demo.glade

demo.xml:	demo.glade
		gtk-builder-convert demo.glade demo.xml

clean:
		rm -f *~ *.o *.bak *.gladep
