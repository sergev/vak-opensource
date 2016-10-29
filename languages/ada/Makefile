CC		= gcc-3.3
GNATMAKE	= gnatmake --GCC=$(CC) -Pgnat.gpr

ALL		= echo hello

all:		$(ALL)

hello:		hello.adb gnat.gpr
		$(GNATMAKE) $@

echo:		echo.adb gnat.gpr
		$(GNATMAKE) $@

hello.o:	hello.adb
		$(ADAC) $(ADAFLAGS) -c $<

.ada.o:
		$(ADAC) $(ADAFLAGS) -c $<

clean:
		rm -f *~ *.o *.ali $(ALL) b_*.c

.SUFFIXES:	.ada
