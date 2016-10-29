SHELL   = /bin/sh
INSTALL = /usr/SVSB
CFLAGS  = -O -DFLEXNAMES
LDFLAGS =
cpp     = cpp

all:    cpp

cpp:    cpp.o cpy.o
	$(CC) $(LDFLAGS) -o cpp cpp.o cpy.o

cpy.c:  cpy.y
	$(YACC) cpy.y
	mv y.tab.c cpy.c

cpy.o:  cpy.c yylex.c
	$(CC) $(CFLAGS) -c cpy.c

cpp.o:  cpp.c
	$(CC) $(CFLAGS) -Dunix -Dsvsb -c cpp.c

clean:
	rm -f *.o *.b a.out core cpy.c cpp

install: $(INSTALL)/lib/$(cpp)

$(INSTALL)/lib/$(cpp): cpp
	cp cpp $(INSTALL)/lib/$(cpp)
	strip $(INSTALL)/lib/$(cpp)
