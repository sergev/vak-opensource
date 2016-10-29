CC		= gcc -Wall
CXX		= g++ -Wall
GOC		= gccgo -Wall
CFLAGS		= -O -fpic -I/home/vak/go/GCC/gccgo/libgo/runtime -fno-stack-protector
CXXFLAGS	= $(CFLAGS) -fno-exceptions -fno-rtti -Wno-non-virtual-dtor
GOFLAGS		= $(CFLAGS) -fno-exceptions -fno-split-stack
BXFLAGS		= -static -nostdlib -Wl,-Tbxscript.x,-e_start -fpic
GOLDFLAGS	= -static -nostdlib -Wl,-Tbxscript.x,-ego.main.Start -fpic

all:		shell hello_c.ex hello_go.ex hello_cpp.ex

shell:		shell.cpp
		$(CXX) $(LDFLAGS) -o $@ shell.cpp

hello_cpp.ex:	hello_cpp.o
		$(CC) $(BXFLAGS) hello_cpp.o -o $@
		objdump -D -S $@ > $*.dis

hello_c.ex:	hello_c.o debug.o
		$(CC) $(BXFLAGS) hello_c.o debug.o -o $@ -lc
		objdump -D -S $@ > $*.dis

hello_go.ex:	stream.o hello_go.o libgo.o
		$(CC) $(GOLDFLAGS) hello_go.o libgo.o stream.o -o $@
		objdump -D -S $@ > $*.dis

clean:
		rm -f *~ *.o *.s *.o *.ex *.dis shell

.SUFFIXES:	.o .ex .dis .go

.c.o:
		$(CC) $(CFLAGS) -c $< -o $@

.c.s:
		$(CC) $(CFLAGS) -S $<

.cpp.o:
		$(CXX) $(CXXFLAGS) -c $< -o $@

.cpp.s:
		$(CXX) $(CXXFLAGS) -S $<

.go.o:
		$(GOC) $(GOFLAGS) -c $< -o $@

.go.s:
		$(GOC) $(GOFLAGS) -S $<

.ex.dis:
		objdump -D -S $? > $@
