all:  compile link hex

DEVICE = 32MX795F512H

IMAGE = maximite_console

DEFINES = -DMAXIMITE

ifeq ($(OS),Windows_NT)
  XC32_HOME = c:/xc32
else
  XC32_HOME = /Applications/microchip/xc32/v1.00
endif

CC = $(XC32_HOME)/bin/xc32-gcc

CC_OPTS = \
  -mprocessor=$(DEVICE) \
  $(DEFINES) \
  -Wall -O1

LINKER = $(XC32_HOME)/bin/xc32-gcc

LINKER_OPTS = \
  -mprocessor=$(DEVICE) \
  -Wl,--script=./Maximite.ld

FILES = \
  ./video.c \
  ./keyboard.c \
  ./main.c \

OBJS = $(subst .c,.o,$(FILES))

%.o : %.c
	$(CC) $(CC_OPTS) -c $< -o $@

compile: $(OBJS)

link:
	$(LINKER) $(OBJS) $(LINKER_OPTS) -o $(IMAGE).out

hex:
	$(XC32_HOME)/bin/xc32-bin2hex $(IMAGE).out

load:
	./mphidflash -v 04D8 -p FA8D -n -w $(IMAGE).hex

clean:
	-rm $(OBJS)
	-rm $(IMAGE).out $(IMAGE).hex

zip:
	7z a -r maximite-console-source.zip $(FILES) Makefile Maximite.ld
