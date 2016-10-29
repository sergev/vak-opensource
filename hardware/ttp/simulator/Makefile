CC		= gcc -Wall -g
CFLAGS		=
GTKFLAGS	= $(shell pkg-config libglade-2.0 --cflags)
GTKLIBS		= $(shell pkg-config libglade-2.0 --libs)
OBJS		= simulator.o ctlr.o bus.o node0.o node1.o node_common.o crc32-ipmce.o

all:		sim gsim

sim:		main.o $(OBJS)
		$(CC) -o $@ main.o $(OBJS) $(GTKLIBS)

gsim:		main-gtk.o $(OBJS)
		$(CC) -o $@ main-gtk.o $(OBJS) $(GTKLIBS)

main-gtk.o:	main-gtk.c
		$(CC) -I.. $(GTKFLAGS) -c $<

clean:
		rm -f *~ *.bak *.o simulator.gladep sim gsim
		$(MAKE) -C simulink $@

edit:
		glade-gtk2 simulator.glade &
