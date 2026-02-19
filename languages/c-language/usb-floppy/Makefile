UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)

ifeq ($(UNAME_S),Linux)
all: usb-floppy-info usb-floppy-format
else
all: usb-floppy-info
endif

usb-floppy-info: usb-floppy-info4.o
	$(CC) $(LDFLAGS) $< -o $@

usb-floppy-format: usb-floppy-format.c
	$(CC) $(CFLAGS) -Wall -Wextra -o $@ $<
