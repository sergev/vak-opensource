CC		= gcc -Wall -g
CFLAGS 		= -O
LDFLAGS		= -framework CoreAudio
OBJS		= sound-meter.o

all:		sound-meter

sound-meter:	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@

clean:
		rm -f *~ *.o sound-meter
