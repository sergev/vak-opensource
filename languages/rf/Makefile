
rf:     lex.yy.c y.tab.c
	cc y.tab.c -ly -ll -o rf

debug:  lex.yy.c y.deb.c
	cc y.deb.c -DYYDEBUGON -ly -ll -o debug

y.tab.c: y.y
	yacc $?

y.deb.c: y.y
	yacc -v $?
	mv y.tab.c y.deb.c

lex.yy.c: l.l
	lex $?

clean:
	rm -f y.tab.* lex.yy.*
