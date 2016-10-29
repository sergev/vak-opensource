#
# Remote login utility, based on UDP protocol.
#
# Copyright (C) 1994 Cronyx Ltd.
# Authors: Serge Vakulenko, <vak@cronyx.msk.su>
#          Sergey Ryzhkov, <sir@cronyx.msk.su>
# Date: Wed Jul 27 21:27:58 MSD 1994
#
CFLAGS  = -O -Wall
ALL     = udplogin udplogind

all:    $(ALL)

udplogin: udplogin.o
	$(CC) $(LDFLAGS) -o udplogin udplogin.o

udplogind: udplogind.o
	$(CC) $(LDFLAGS) -o udplogind udplogind.o

udptty: udptty.o
	$(CC) $(LDFLAGS) -o udptty udptty.o

clean:
	rm -f *.[ob] $(ALL)

install: all
	install -c -s udplogind /usr/local/etc/udplogind
	install -c -s udplogin /usr/local/bin/udplogin
