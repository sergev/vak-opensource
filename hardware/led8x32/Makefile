CFLAGS  = -Wall -Werror -O2
OBJS    = ch341a.o max7219.o
LIBS    = -lusb-1.0
PROG    = clock madfly

all:    $(PROG)

clock:  clock.c $(OBJS)
	gcc $(CFLAGS) clock.c $(OBJS) $(LIBS) -o $@

madfly: madfly.c $(OBJS)
	gcc $(CFLAGS) madfly.c $(OBJS) $(LIBS) -o $@

clean:
	rm -f *.o $(PROG)

install-udev-rule:
	cp 99-ch341a-prog.rules /etc/udev/rules.d/
	udevadm control --reload-rules

.PHONY: clean install-udev-rule

###
ch341a.o: ch341a.c ch341a.h
clock.o: clock.c max7219.h
madfly.o: madfly.c max7219.h
max7219.o: max7219.c max7219.h ch341a.h
