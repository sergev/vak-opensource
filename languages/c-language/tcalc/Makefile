# Makefile for TCALC - Spreadsheet application converted to ncurses
# Original: Borland C++ 2.0 DOS version
# Converted to: ncurses for Unix/Linux

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Werror
LDFLAGS = -lncurses

# Source files
SOURCES = tcalc.c tcdisply.c tcinput.c tcommand.c tcparser.c spreadsheet.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = tcalc

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
%.o: %.c tcalc.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install (optional - copies to /usr/local/bin)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Phony targets
.PHONY: all clean install uninstall

