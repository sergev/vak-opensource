INSTALL       = /usr/SVSB
AR            = var
AS            = vas
CC            = vcc
LORDER        = vlorder
RANLIB        = vranlib
PRINT	      = pr

C             = -I../include

DEST          = $(INSTALL)/lib

EXTHDRS	      =

HDRS	      =

LIBRARY       = libtst.a

MAKEFILE      = Makefile

OBJS	      = abs.o\
		atof.o\
		atof.o\
		atoi.o\
		atol.o\
		csv.o\
		ctype.o\
		index.o\
		memcpy.o\
		qsort.o\
		rand.o\
		rindex.o\
		setjmp.o\
		strcat.o\
		strcatn.o\
		strchr.o\
		strcmp.o\
		strcmpn.o\
		strcpy.o\
		strcpyn.o\
		strlen.o\
		strncat.o\
		strncmp.o\
		strncpy.o\
		strpbrk.o\
		strrchr.o\
		strspn.o\
		strtok.o\
		u_div_u.o\
		u_mod_u.o\
		u_mul_u.o

SRCS	      = abs.c\
		atof.c\
		atof.s\
		atoi.c\
		atol.c\
		csv.s\
		ctype.c\
		index.c\
		memcpy.c\
		qsort.c\
		rand.c\
		rindex.c\
		setjmp.s\
		strcat.c\
		strcatn.c\
		strchr.c\
		strcmp.c\
		strcmpn.c\
		strcpy.c\
		strcpyn.c\
		strlen.c\
		strncat.c\
		strncmp.c\
		strncpy.c\
		strpbrk.c\
		strrchr.c\
		strspn.c\
		strtok.c\
		u_div_u.s\
		u_mod_u.s\
		u_mul_u.s

all:            $(OBJS)

$(LIBRARY):	$(OBJS)
		$(AR) cru $(LIBRARY) $(OBJS)

clean:;         rm -f *.o *.b a.out core *.a

depend:;        mkmf -f $(MAKEFILE) LIBRARY=$(LIBRARY) DEST=$(DEST)

index:;         ctags -wx $(HDRS) $(SRCS)

install:        $(OBJS)
		rm -f $(DEST)/$(LIBRARY)
		$(AR) cru $(DEST)/$(LIBRARY) $(OBJS)
		$(RANLIB) $(DEST)/$(LIBRARY)

library:        $(LIBRARY)

print:;         $(PRINT) $(HDRS) $(SRCS)

tags:           $(HDRS) $(SRCS); ctags $(HDRS) $(SRCS)

.c.o:
	$(CC) $(C) -c $<
###
