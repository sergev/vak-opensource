CFLAGS          = -Wall -Werror
UNAME           = $(shell uname)

ifeq ($(UNAME),Linux)
    # Use 'sudo port install libusb-0.1-dev'
    LIBS        += -Wl,-Bstatic -lusb -Wl,-Bdynamic
endif
ifeq ($(UNAME),Darwin)
    # Use 'sudo port install libusb-legacy'
    #CC          += -arch i386 -arch x86_64
    CFLAGS      += -I/opt/local/include/libusb-legacy
    LIBS        += -framework IOKit -framework CoreFoundation
    LIBS        += /opt/local/lib/libusb-legacy/libusb-legacy.a
endif

demo:           demo.o ft232r.o
		$(CC) $(LDFLAGS) demo.o ft232r.o $(LIBS) -o $@

clean:
		rm -f *.o demo
