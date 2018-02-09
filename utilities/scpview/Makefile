PROG            = scp_view
OBJS            = scp-file.o scp-view.o

CFLAGS          = -O -g -Wall -Werror
LDFLAGS         = -g

all:            $(PROG)

$(PROG):        $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
		$(RM) *.o scp_view
