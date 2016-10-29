TOPSRC          = $(shell cd ../..; pwd)
include $(TOPSRC)/target.mk

CFLAGS          += -O -Wall -Werror

OBJS    	= malloc-alternate.o

all:    	../libmalt.a

../libmalt.a:   ${OBJS}
		@$(AR) cru $@ ${OBJS}
		$(RANLIB) $@

install: 	all

clean:
		rm -f *.o a.out core test errs ../libmalt*.a
