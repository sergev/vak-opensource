#LIBS = -lusb-1.0

all: usb-floppy-info usb-floppy-format

usb-floppy-info: usb-floppy-info2.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@

usb-floppy-format: usb-floppy-format.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@
