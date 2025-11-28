CPP=cpp
CC=c86
AS=as
LD=ld

INCLUDES=-I/usr/include
DEFINES=

CPPFLAGS=-0 $(INCLUDES) $(DEFINES)
CFLAGS=-g -O -bas86 -warn=4 -lang=c99
CFLAGS+=-align=yes -separate=yes -stackopt=minimum -peep=all -stackcheck=no
ASFLAGS=-0 -j
LDFLAGS=-0 -i -L/usr/lib
LDLIBS=-lc86

all: hello

hello: hello.c
	$(CPP) $(CPPFLAGS) -o $*.i $^
	$(CC) $(CFLAGS) $*.i $*.s
	$(AS) $(ASFLAGS) -o $*.o -l $*.lst $*.s
	$(LD) $(LDFLAGS) -o $@ $*.o $(LDLIBS)

clean:
	rm -f *.o *.s *.i *.lst hello
