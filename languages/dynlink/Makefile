CC		= gcc411 -Wall
ADAC		= gcc411 -Wall -gnat05
CXX		= g++411 -Wall
CFLAGS		= -O -fpic
ADAFLAGS	= $(CFLAGS) -gnatp
GNATBIND	= /usr/local/gcc411/bin/gnatbind
CXXFLAGS	= $(CFLAGS) -fno-exceptions -fno-rtti -Wno-non-virtual-dtor
BXFLAGS		= -static -nostdlib -Wl,-Tbxscript.x,--defsym,__gnat_eh_personality=0 -fpic

all:		shell hello_cpp.ex hello_ada.ex

shell:		shell.o
		$(CC) $(LDFLAGS) -o $@ shell.o

hello_cpp.ex:	hello_cpp.po
		$(CC) $(BXFLAGS) hello_cpp.po -o $@
		objdump -D -S $@ > $*.dis

hello_ada.ex:	hello_ada.adb hello_ada.ads
		$(ADAC) $(ADAFLAGS) -c $<
		$(CC) $(BXFLAGS) -e $*__start $*.o -o $@
		objdump -D -S $@ > $*.dis

clean:
		rm -f *~ *.o *.s *.po *.ex *.dis *.ali shell

.SUFFIXES:	.po .ex .dis .adb

.cpp.po:
		$(CXX) $(CXXFLAGS) -c $< -o $@

.cpp.s:
		$(CXX) $(CXXFLAGS) -S $<

.adb.s:
		$(ADAC) $(ADAFLAGS) -S $<

.ex.dis:
		objdump -D -S $? > $@
