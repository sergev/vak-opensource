EXECUTABLE      = gpio
OBJECTS         = main.o libmcp2221.o
CFLAGS          = -g -Wall -O -I.
LDFLAGS         = -g
CC              = gcc
UNAME           = $(shell uname)

# Linux
ifeq ($(UNAME),Linux)
    LDLIBS      = -Wl,-Bstatic -lhidapi-libusb -lusb-1.0 -Wl,-Bdynamic -ludev -lpthread
endif

# Mac OS X
ifeq ($(UNAME),Darwin)
    LDLIBS      = -lhidapi -framework IOKit -framework CoreFoundation
endif

all:            $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

clean:
		rm -rf *.o $(EXECUTABLE)

.PHONY:         clean all
