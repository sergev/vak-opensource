PROGS		= maze-gen robot-gen robot-maze robot-tour robot-copy
LIBS		=
CFLAGS		= -O3
LDFLAGS		=
CC		= gcc -Wall -g

all:		$(PROGS)

maze-gen:	maze-gen.o maze.o
		$(CC) $(LDFLAGS) -o maze-gen maze-gen.o maze.o $(LIBS)

robot-gen:	robot-gen.o robot.o
		$(CC) $(LDFLAGS) -o robot-gen robot-gen.o robot.o $(LIBS)

robot-maze:	robot-maze.o robot.o maze.o
		$(CC) $(LDFLAGS) -o robot-maze robot-maze.o robot.o maze.o $(LIBS)

robot-copy:	robot-copy.o robot.o
		$(CC) $(LDFLAGS) -o robot-copy robot-copy.o robot.o $(LIBS)

robot-tour:	robot-tour.o robot.o maze.o
		$(CC) $(LDFLAGS) -o robot-tour robot-tour.o robot.o maze.o $(LIBS)

clean:
		rm -f *.o *~ *.bak $(PROGS)

ChangeLog:	.
		cvs2cl --passwd /etc/passwd

maze3:		maze-gen
		for seed in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20; do\
			maze-gen -s $$seed 3 > 3-$$seed.maze;\
		done
