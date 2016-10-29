PROG    = all-leds backlight cube demo demo2

OBJS    = linkit.o draw.o text.o fireworks.o ripples.o \
          sparkle.o worm.o sinelines.o randsend.o rain.o \
          filler.o blinky.o plane.o pyramid.o intro.c patharound.o \
          slanted.o curved.o balls.o wavy.o

CFLAGS  = -O -g -Wall
LIBS    = -lmraa

all:    $(PROG)

all-leds: all-leds.o linkit.o
	cc $(LDFLAGS) $@.o linkit.o $(LIBS) -o $@

backlight: backlight.o linkit.o
	cc $(LDFLAGS) $@.o linkit.o $(LIBS) -o $@

cube:   cube.o linkit.o
	cc $(LDFLAGS) $@.o linkit.o $(LIBS) -o $@

demo:   demo.o $(OBJS)
	cc $(LDFLAGS) $@.o $(OBJS) $(LIBS) -lm -o $@

demo2:  demo2.o $(OBJS)
	cc $(LDFLAGS) $@.o $(OBJS) $(LIBS) -lm -o $@

clean:
	rm -f *.o *~ $(PROG)

###
all-leds.o: all-leds.c cube.h
backlight.o: backlight.c cube.h
balls.o: balls.c cube.h
blinky.o: blinky.c cube.h
cube.o: cube.c cube.h
curved.o: curved.c cube.h
demo2.o: demo2.c cube.h
demo.o: demo.c cube.h
draw.o: draw.c cube.h
filler.o: filler.c cube.h
fireworks.o: fireworks.c cube.h
intro.o: intro.c cube.h
linkit.o: linkit.c cube.h
patharound.o: patharound.c cube.h
plane.o: plane.c cube.h
pyramid.o: pyramid.c cube.h
rain.o: rain.c cube.h
randsend.o: randsend.c cube.h
ripples.o: ripples.c cube.h
sinelines.o: sinelines.c cube.h
slanted.o: slanted.c cube.h
sparkle.o: sparkle.c cube.h
text.o: text.c cube.h
wavy.o: wavy.c cube.h
worm.o: worm.c cube.h
