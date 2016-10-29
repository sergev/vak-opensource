CC		= cc -Wall -g
CFLAGS		= -O
OBJ		= parser.o scanner.o compiler.o machdep.o
INSTDIR		= /usr/local
YACC		= byacc -d

all:		cc1 cc17

cc1:		$(OBJ)
		$(CC) $(LDFLAGS) $(OBJ) -o $@ -ll

cc17:		cc.o
		$(CC) $(LDFLAGS) cc.o -o $@

clean:
		rm -f cc1 cc17 test_lex test_gram *.tmp *.o *~ y.tab.h scanner.c a.out

install:	cc1 cc17
		install -d $(INSTDIR)/lib/pic17
		install -c cc1 $(INSTDIR)/lib/pic17/cc1
		install -c cc17 $(INSTDIR)/bin/cc17

test:		test_lex test_gram
		test_lex < lex.tst > lex.tmp
		diff -u lex.out lex.tmp
		test_gram < gram.tst

test_lex:	test_lex.o scan.o
		$(CC) $(LDFLAGS) test_lex.o scan.o -o test_lex -ll

test_gram:	test_gram.o scan.o comp.o
		$(CC) $(LDFLAGS) test_gram.o scan.o comp.o -o test_gram -ll

test_lex.o:	grammar.y
		$(YACC) grammar.y
		$(CC) $(CFLAGS) -c -DDEBUG_LEX y.tab.c -o test_lex.o
		rm -f y.tab.c

test_gram.o:	grammar.y
		$(YACC) grammar.y
		$(CC) $(CFLAGS) -c -DDEBUG_GRAM y.tab.c -o test_gram.o
		rm -f y.tab.c

compiler.o:	compiler.c global.h machdep.h
machdep.o:	machdep.c global.h machdep.h
parser.o:	parser.y global.h machdep.h
scanner.o:	scanner.l global.h machdep.h parser.o
cc17:		cc.o
