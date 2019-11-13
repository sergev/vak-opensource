CC      = gcc
CFLAGS  = -g -O2 -Wall -Werror

TARGET  = spi-demo
OBJ     = gpio.o spi.o alt.o

all:    $(TARGET)

clean:
	rm -f *.o $(TARGET)

spi-demo: spi-demo.o $(OBJ)
	$(CC) $(LDFLAGS) spi-demo.o $(OBJ) -o $@
###
