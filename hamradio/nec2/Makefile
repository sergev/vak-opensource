
#--- Mac OS X, g95
#FC              = g95 -fsecond-underscore -fsloppy-char
#VERSION         = $(shell $(FC) --version | head -1)

#--- Linux, gfortran
FC              = gfortran -fno-automatic -fsecond-underscore
VERSION         = $(shell $(FC) --version | head -1)

#--- Linux, f2c and fort77
#FC              = fort77
#VERSION         = $(shell f2c --version)

CFLAGS          = -O -Wall

PROG            = nec2dxs
OBJS            = $(PROG).o get_time.o

all:            $(PROG)

$(PROG):        $(OBJS)
		$(FC) $(OBJS) -o $@

$(PROG).o:      $(PROG).f
		echo "      DATA G77PORT/'$(VERSION)'/" > g77port.inc
		$(FC) $(CFLAGS) -c $< -o $@

clean:
		rm -f $(PROG) *.o

example1:       $(PROG) example1.nec
		(echo example1.nec; echo example1.out) | ./$(PROG)

gwave:          $(PROG) Gwave.nec
		(echo Gwave.nec; echo Gwave.out) | ./$(PROG)
