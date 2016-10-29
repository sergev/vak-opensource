AS            = /usr/SVSB/bin/as

CC            = /usr/SVSB/bin/cc

DEST	      = /usr/SVSB/lib

EXTHDRS	      =

HDRS	      =

LIBRARY	      = libcrt.o

MAKEFILE      = Makefile

OBJS	      = abs.o\
		atof.o\
		atoi.o\
		atol.o\
		csv.o\
		ctype.o\
		index.o\
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
		u_add_u.o\
		u_div_u.o\
		u_mod_u.o\
		u_mul_u.o

PRINT	      = pr

SRCS	      = abs.c\
		atof.c\
		atoi.c\
		atol.c\
		csv.s\
		ctype.c\
		index.c\
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
		u_add_u.s\
		u_div_u.s\
		u_mod_u.s\
		u_mul_u.s

all:            $(OBJS)

$(LIBRARY):	$(OBJS)
		ar cru $(LIBRARY) $(OBJS)

clean:;         rm -f $(OBJS)

depend:;        mkmf -f $(MAKEFILE) LIBRARY=$(LIBRARY) DEST=$(DEST)

index:;         ctags -wx $(HDRS) $(SRCS)

install:        $(OBJS)

library:        $(LIBRARY)

print:;         $(PRINT) $(HDRS) $(SRCS)

tags:           $(HDRS) $(SRCS); ctags $(HDRS) $(SRCS)
###
