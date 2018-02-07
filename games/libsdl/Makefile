CC      = cc
CFLAGS  = -Wall -Werror -g -I. -DSDL $(shell sdl2-config --cflags)
LIBS    = $(shell sdl2-config --libs)
PROG    = flappy pong
OBJS    = lucidasans15.o lucidasans28.o gpanel-sdl2.o input.c

all:    $(PROG)

clean:
	rm -f *.o *~ $(PROG)

flappy: flappy.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) flappy.c $(OBJS) $(LIBS)

pong:   pong.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) pong.c $(OBJS) $(LIBS)

lucidasans15.o: lucidasans15.c
	$(CC) $(CFLAGS) -o $@ -c $?

lucidasans28.o: lucidasans28.c
	$(CC) $(CFLAGS) -o $@ -c $?
