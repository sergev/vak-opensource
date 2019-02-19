CFLAGS          = -Wall -Werror
LDLIBS          = -lyaml
PROG            = token-parser event-parser document-parser

all:            $(PROG)

token-parser:   token-parser.c

event-parser:   event-parser.c

document-parser: document-parser.c

clean:
		rm -f $(PROG)
