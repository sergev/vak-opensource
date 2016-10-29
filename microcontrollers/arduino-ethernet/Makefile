TOPSRC          = /scratch/retrobsd/trunk
include $(TOPSRC)/target.mk

CFLAGS          += -O -Wall -Werror

OBJCLIENT       = w5100.o socket.o ethernet.o client.o
OBJSERVER       = $(OBJCLIENT) server.o
LIBS            +=
EXAMPLES        = example-telnet-client \
                  example-udp-ntp-client \
                  example-chat-server

all:            $(EXAMPLES)

example-telnet-client: ${OBJCLIENT} example-telnet-client.o
		${CC} ${LDFLAGS} -o $@.elf ${OBJCLIENT} $@.o ${LIBS}
		${OBJDUMP} -S $@.elf > $@.dis
		${SIZE} $@.elf
		${ELF2AOUT} $@.elf $@

example-udp-ntp-client: ${OBJCLIENT} udp.o example-udp-ntp-client.o
		${CC} ${LDFLAGS} -o $@.elf ${OBJCLIENT} udp.o $@.o ${LIBS}
		${OBJDUMP} -S $@.elf > $@.dis
		${SIZE} $@.elf
		${ELF2AOUT} $@.elf $@

example-chat-server: ${OBJSERVER} example-chat-server.o
		${CC} ${LDFLAGS} -o $@.elf ${OBJSERVER} $@.o ${LIBS}
		${OBJDUMP} -S $@.elf > $@.dis
		${SIZE} $@.elf
		${ELF2AOUT} $@.elf $@

clean:
		rm -f *.o *.elf *.dis $(EXAMPLES)

install:        all
		install example-telnet-client $(DESTDIR)/bin/
