##########################################################################
#                                                                        #
#   @@     @@@    @   @    @@@@    @  @    @@@@@   @  @    @@@     @@@@  #
#  @  @    @  @   @   @    @       @@ @      @     @  @    @  @    @     #
#  @  @    @  @   @   @    @@@     @ @@      @     @  @    @@@     @@@   #
#  @@@@    @  @    @ @     @       @  @      @     @  @    @  @    @     #
#  @  @    @@@      @      @@@@    @  @      @      @@     @  @    @@@@  #
#                                                                        #
#                      ИНИЦИАЛИЗАЦИЯ БАЗЫ ДАННЫХ                         #
#                                                                        #
##########################################################################

CFLAGS=-g
LDFLAGS=-g
NAME=ini

IOBJS= init_adv.o  getlin.o  getobj.o  iactio.o  icave.o   iclass.o  \
       ievent.o    imessa.o  iobjec.o  ivocab.o  putcnd.o  putmes.o  \
       screen.o    fatal.o   stat.o                                  \
       ../common/savecm.o  ../common/vocab.o

../$(NAME): $(IOBJS) Makefile
	cc -o ../$(NAME) $(LDFLAGS) $(IOBJS)


stat.o:  ../common/adv_common

imessa.o  iobjec.o  iactio.o  icave.o   iclass.o  \
ievent.o  ivocab.o  putcnd.o  putmes.o  getlin.o  \
getobj.o: adv_ini.h ../common/adv_common

../common/savecm.o:  ../common/savecm.c  ../common/adv_common
	(cd ../common; cc -c $(CFLAGS) savecm.c)

../common/vocab.o:   ../common/vocab.c   ../common/adv_common
	(cd ../common; cc -c $(CFLAGS) vocab.c)


clear:
	-rm -f *.b *~
	-rm -f *.o
	-rm -f ../common/*.b ../common/*~
	-rm -f ../common/*.o
