
EXECUTABLE      = gpio

OBJECTS         = main.o libmcp2221.o

CFLAGS          = -g -Wall -O -I.

LDFLAGS         = -g

LDLIBS          = -Wl,-Bstatic -lhidapi-libusb -lusb-1.0 -Wl,-Bdynamic -ludev -lpthread

CC              = gcc

all:            $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

clean:
		rm -rf *.o $(EXECUTABLE)

.PHONY:         clean all
