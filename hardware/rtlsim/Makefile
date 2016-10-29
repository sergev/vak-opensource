CFLAGS          = -Wall -Werror -g -O
LDFLAGS         = -g
OBJS            = example.o rtlsim.o

all:            example sum random.c random.v random

example:        example.o rtlsim.o
		$(CC) $(LDFLAGS) -o $@ $@.o rtlsim.o

sum:            sum.o rtlsim.o
		$(CC) $(LDFLAGS) -o $@ $@.o rtlsim.o

random:         random.o rtlsim.o
		$(CC) $(LDFLAGS) -o $@ $@.o rtlsim.o

clean:
		rm -f *.o a.out ucli.key example sum random simv

random.c:       mkrandom-c.pl
		./mkrandom-c.pl > $@

random.v:       mkrandom-verilog.pl
		./mkrandom-verilog.pl > $@
