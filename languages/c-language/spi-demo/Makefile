CC      = gcc
CFLAGS  = -g -O2 -Wall -Werror

TARGET  = spi-demo
#OBJ     = bmp.o font16.o font20.o font24.o lcd.o paint.o spi.o
OBJ     = gpio.o spi.o #lcd.o

all:    $(TARGET)

clean:
	rm -f *.o $(TARGET)

spi-demo: spi-demo.o $(OBJ)
	$(CC) $(LDFLAGS) spi-demo.o $(OBJ) -o $@
###
