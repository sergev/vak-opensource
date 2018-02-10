PROG            = scp_view
OBJS            = scp.o main.o vcd.o

GITCOUNT        = $(shell git rev-list HEAD --count)$(if $(shell git ls-files -m),m)

CFLAGS          = -O -g -Wall -Werror -DGITCOUNT='"$(GITCOUNT)"'
LDFLAGS         = -g

all:            $(PROG)

$(PROG):        $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
		$(RM) *.o scp_view
###
main.o: main.c scp.h
scp.o: scp.c scp.h
vcd.o: vcd.c scp.h
