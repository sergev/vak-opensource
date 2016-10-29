CFLAGS          = -Wall -Wno-unused-function -DDEBUG -O
CC              = gcc -g

OBJS            = scm.o func.o
TESTS           = cxr int list quote t member

all:            scm

scm:            $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o scm

tests:          scm $(TESTS)
		for i in $(TESTS); do scm -v $$i > $$i.out;\
		diff $$i.std $$i.out; done

newtests:       scm $(TESTS)
		for i in $(TESTS); do scm -v $$i > $$i.std; done

clean:;         rm -f *.o *.b .,* core.* core a.out scm *.out

###
$(OBJS): scm.h
