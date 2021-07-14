CA      = ca
AFLAGS  = -u AAAAAAA
LIBS    =
PROG    = hello gcd factorial fibonacci sieve

all:    $(PROG)

%:      %.a68
	$(CA) $(AFLAGS) $<

clean:
	rm -f $(PROG) *.o *.c
