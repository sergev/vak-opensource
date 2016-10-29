SHELL   = /bin/sh
CC      = vcc
INC     = ../../h
INCSYS  = $(INC)/sys
CFLAGS  = -x -I$(INC)

OBJS    = bufctl.o clrerr.o ctermid.o data.o doscan.o exit.o fdopen.o\
		fgetc.o fgets.o filbuf.o findiop.o flsbuf.o fopen.o fputc.o\
		fputs.o fread.o fseek.o ftell.o fwrite.o getchar.o\
		gets.o getw.o putchar.o puts.o putw.o rew.o scanf.o\
		setbuf.o setvbuf.o tempnam.o tmpfile.o tmpnam.o\
		ungetc.o

ALL     = bufctl.o clrerr.o ctermid.o cuserid.o data.o doscan.o fdopen.o\
		fgetc.o fgets.o filbuf.o findiop.o flsbuf.o fopen.o fputc.o\
		fputs.o fread.o fseek.o ftell.o fwrite.o getchar.o getpass.o\
		gets.o getw.o popen.o putchar.o puts.o putw.o rew.o scanf.o\
		setbuf.o setvbuf.o system.o tempnam.o tmpfile.o tmpnam.o\
		ungetc.o

all:    $(OBJS)

clean:
	-rm -f *.o *.b a.out core

fgets.o flsbuf.o fputs.o fread.o fwrite.o gets.o puts.o: stdiom.h
doscan.o: $(INC)/ctype.h
fopen.o: $(INC)/fcntl.h
cuserid.o: $(INC)/pwd.h
getpass.o popen.o system.o: $(INC)/signal.h $(INCSYS)/signal.h
clrerr.o ctermid.o cuserid.o data.o doscan.o fdopen.o fgetc.o fgets.o\
filbuf.o findiop.o flsbuf.o fopen.o fputc.o fputs.o fread.o fseek.o ftell.o\
fwrite.o getchar.o getpass.o gets.o getw.o popen.o putchar.o puts.o putw.o\
rew.o scanf.o setbuf.o tempnam.o tmpfile.o tmpnam.o ungetc.o: $(INC)/stdio.h
tempnam.o: $(INC)/string.h
fdopen.o flsbuf.o: $(INCSYS)/errno.h
getpass.o: $(INC)/termio.h $(INCSYS)/termio.h
doscan.o: $(INC)/values.h
doscan.o scanf.o: $(INC)/varargs.h
