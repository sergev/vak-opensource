SHELL   = /bin/sh
CC      = vcc
INC     = ../../h
CFLAGS  = -x -I$(INC)
INCSYS  = $(INC)/sys

OBJS    = a64l.o abs.o assert.o atof.o atoi.o atol.o bsearch.o\
		calloc.o crypt.o ctype.o\
		ecvt.o errlst.o frexp.o gcvt.o\
		getenv.o\
		getopt.o\
		isatty.o l3.o ldexp.o lfind.o lsearch.o\
		malloc.o memccpy.o memchr.o memcmp.o memcpy.o memset.o\
		mktemp.o modf.o perror.o putenv.o\
		qsort.o strcat.o strchr.o strcmp.o\
		strcpy.o strcspn.o strlen.o strncat.o strncmp.o strncpy.o\
		strpbrk.o strrchr.o strspn.o strtod.o strtok.o strtol.o\
		swab.o tell.o tolower.o toupper.o

ALL     = a64l.o abort.o abs.o assert.o atof.o atoi.o atol.o bsearch.o\
		calloc.o clock.o crypt.o ctime.o ctype.o dial.o drand48.o\
		ecvt.o errlst.o execvp.o frexp.o ftok.o ftw.o gcvt.o\
		getcwd.o getenv.o getgrent.o getgrgid.o getgrnam.o getlogin.o\
		getopt.o getpw.o getpwent.o getpwnam.o getpwuid.o getut.o\
		hsearch.o isatty.o l3.o l64a.o ldexp.o lfind.o lsearch.o\
		malloc.o memccpy.o memchr.o memcmp.o memcpy.o memset.o\
		mktemp.o modf.o mon.o nlist.o perror.o putenv.o putpwent.o\
		qsort.o rand.o sleep.o ssignal.o strcat.o strchr.o strcmp.o\
		strcpy.o strcspn.o strlen.o strncat.o strncmp.o strncpy.o\
		strpbrk.o strrchr.o strspn.o strtod.o strtok.o strtol.o\
		swab.o tell.o tfind.o tolower.o toupper.o tsearch.o ttyname.o\
		ttyslot.o

all: $(OBJS)

grep:
	grep include `echo $(OBJS)|sed 's/\.o/.c/g'` >grep

clean:
	-rm -f *.o *.b a.out core

nlist.o:                                $(INC)/a.out.h $(INC)/ar.h
atof.o ctype.o getpw.o strtol.o:        $(INC)/ctype.h
dial.o:                                 $(INC)/dial.h $(INCSYS)/termio.h
dial.o ftw.o getut.o:                   $(INC)/errno.h $(INCSYS)/errno.h
dial.o:                                 $(INC)/fcntl.h
ftw.o:                                  $(INC)/ftw.h
getgrent.o getgrgid.o getgrnam.o:       $(INC)/grp.h
mon.o:                                  $(INC)/mon.h
ecvt.o frexp.o ldexp.o modf.o:          $(INC)/nan.h
getpwent.o getpwnam.o getpwuid.o putpwent.o: $(INC)/search.h
tsearch.o tfind.o:                      $(INC)/search.h
abort.o dial.o sleep.o ssignal.o:       $(INC)/signal.h $(INCSYS)/signal.h
dial.o drand48.o getcwd.o getgrent.o getopt.o getpw.o getpwent.o getut.o hsearch.o putpwent.o: $(INC)/string.h
dial.o:                                 $(INC)/string.h
ftw.o ttyname.o:                        $(INCSYS)/dir.h
execvp.o getcwd.o:                      $(INCSYS)/errno.h
clock.o getpwent.o:                     $(INCSYS)/param.h
ftok.o ftw.o getut.o ttyname.o:         $(INCSYS)/stat.h
ftok.o:                                 $(INCSYS)/sysmacros.h
#isatty.o:                              $(INCSYS)/termio.h
clock.o:                                $(INCSYS)/times.h
dial.o:                                 $(INC)/termio.h $(INCSYS)/termio.h
ctime.o:                                $(INC)/time.h
getlogin.o getut.o ttyslot.o:           $(INC)/utmp.h
atof.o ecvt.o l64a.o ldexp.o modf.o strtod.o: $(INC)/values.h
