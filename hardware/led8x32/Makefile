CFLAGS  = -Wall -Werror -O2

test1:  main.c ch341a.c ch341a.h max7219.c max7219.h
	gcc $(CFLAGS) ch341a.c max7219.c main.c -o $@ -lusb-1.0

clean:
	rm *.o test1 -f

install-udev-rule:
	cp 99-ch341a-prog.rules /etc/udev/rules.d/
	udevadm control --reload-rules

.PHONY: clean install-udev-rule
