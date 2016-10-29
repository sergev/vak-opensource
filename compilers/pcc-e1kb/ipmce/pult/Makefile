#
#       Пульт Эльбрус-Б, (C) 1990 Демос/*, Москва
#

#       -DTERMIO        use termio.h instead sgtty.h
#       -DKEYPAD        send 'ks', 'ke' to initialize keypad
#       -DINITFILE      send 'if' file to initialize terminal
#       -DGETWD         use standard getwd () (BSD)
#       -DGETCWD        use standard getcwd () (System V)
#       -DLKEYS         use 'l1' - 'l0' function keys
#       -DDIRENT        use file system independent directory format
#       -DSTDDIR        use standard readdir library
#       -DNOSTRINIT     don't send is, fs when VRestore, VReopen
#       -DHOSTNAME      use gethostname
#       -DDEMO          demo version

# For PC AT Xenix-286
CFLAGS        = -DKEYPAD -DTERMIO -DNOSTRINIT -Me -O
LDFLAGS       = -i

DEST          = /usr/SVSB/bin

EXTHDRS	      = /usr/include/setjmp.h \
		/usr/include/sgtty.h \
		/usr/include/signal.h \
		/usr/include/stdio.h \
		/usr/include/sys/comcrt.h \
		/usr/include/sys/machdep.h \
		/usr/include/sys/pult.h \
		/usr/include/sys/termio.h \
		/usr/include/termio.h

HDRS	      = cap.h \
		dosio.h \
		jlib.h \
		key.h \
		regio.h \
		scr.h

MAKEFILE      = Makefile

OBJS	      = bchan.o \
		cap.o \
		choice.o \
		common.o \
		ctrl.o \
		cvt.o \
		help.o \
		init.o \
		jlib.o \
		key.o \
		mem.o \
		pult.o \
		scr.o \
		tty.o \
		win.o

PROGRAM	      = pult

SRCS	      = bchan.c \
		cap.c \
		choice.c \
		common.c \
		ctrl.c \
		cvt.c \
		help.c \
		init.c \
		jlib.c \
		key.c \
		mem.c \
		pult.c \
		scr.c \
		tty.c \
		win.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:;         rm -f *.o *.b core a.out $(PROGRAM)

depend:;        mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;         ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		cp $(PROGRAM) $(DEST)
		strip $(DEST)/$(PROGRAM)

print:;         pr $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); ctags $(HDRS) $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		make -f $(MAKEFILE) DEST=$(DEST) install
###
bchan.o: regio.h jlib.h scr.h
cap.o: cap.h
choice.o: jlib.h scr.h
common.o: regio.h jlib.h scr.h
ctrl.o: jlib.h scr.h
help.o: jlib.h scr.h
init.o: /usr/include/stdio.h key.h jlib.h scr.h
key.o: key.h cap.h /usr/include/setjmp.h /usr/include/signal.h
mem.o: /usr/include/stdio.h jlib.h scr.h
pult.o: /usr/include/stdio.h /usr/include/sys/pult.h jlib.h scr.h
scr.o: /usr/include/setjmp.h /usr/include/signal.h scr.h key.h cap.h \
	/usr/include/stdio.h
tty.o: /usr/include/termio.h /usr/include/sys/termio.h \
	/usr/include/sys/machdep.h /usr/include/sys/comcrt.h \
	/usr/include/sgtty.h
win.o: /usr/include/stdio.h jlib.h scr.h
