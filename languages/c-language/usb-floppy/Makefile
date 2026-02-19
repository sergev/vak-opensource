#LIBS = -lusb-1.0

all: usb-floppy-info usb-floppy-format mode-select simple-floppy-format probe-vendor-commands ufifmt

usb-floppy-info: usb-floppy-info3.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@

usb-floppy-format: usb-floppy-format.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@

simple-floppy-format: simple-floppy-format.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@

mode-select: mode-select.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@

probe-vendor-commands: probe-vendor-commands.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@

ufifmt: ufifmt.o
	$(CC) $(LDFLAGS) $< $(LIBS) -o $@
