#LIBS = -lusb-1.0

all: usb-floppy-info

usb-floppy-info: usb-floppy-info2.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@
