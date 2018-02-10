PROG            = scp_view
OBJS            = scp.o main.o vcd.o

CFLAGS          = -O -g -Wall -Werror
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
