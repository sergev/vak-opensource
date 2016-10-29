clean:
		rm -f *~ *.dump *.beam

all:

hello:		hello.beam
		erl -noshell -s $@ start -s init stop

fact:		fact.beam
		erl -noshell -s $@ start -s init stop

.SUFFIXES:	.erl .beam

.erl.beam:
		erl -compile $<
