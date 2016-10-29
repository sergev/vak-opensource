CC		= gcc411 -Wall -fno-rtti -Wno-non-virtual-dtor
ADAC		= gcc411 -Wall -gnat05
CFLAGS		= -O -fno-exceptions

all:
		$(CC) $(CFLAGS) -S sequence.cpp
		$(CC) $(CFLAGS) -c sequence.cpp
		$(ADAC) $(CFLAGS) -S -fverbose-asm pkg.adb
		$(ADAC) $(CFLAGS) -c pkg.adb

clean:
		rm -f *~ seq
