##########################################################################
#                                                                        #
#   @@     @@@    @   @    @@@@    @  @    @@@@@   @  @    @@@     @@@@  #
#  @  @    @  @   @   @    @       @@ @      @     @  @    @  @    @     #
#  @  @    @  @   @   @    @@@     @ @@      @     @  @    @@@     @@@   #
#  @@@@    @  @    @ @     @       @  @      @     @  @    @  @    @     #
#  @  @    @@@      @      @@@@    @  @      @      @@     @  @    @@@@  #
#                                                                        #
#                           ИГРА В "ПЕЩЕРУ"                              #
#                                                                        #
##########################################################################

CFLAGS  = -g
LDFLAGS = -g
NAME    = ad

OBJS= advent.o  chnloc.o  dark.o    descr.o  events.o   freeze.o  \
      get.o     indobj.o  motion.o  pct.o    rspeak.o   score.o   \
      specia.o  act.o     yes.o    tolower.o  getans.o  \
      fatal.o   mes.o     mscore.o  conv.o                        \
      ../common/savecm.o  ../common/vocab.o


###########
../$(NAME): $(OBJS) Makefile
	cc -o ../$(NAME) $(LDFLAGS) $(OBJS)

advent.o  chnloc.o  dark.o   descr.o   events.o  freeze.o  indobj.o  \
motion.o  rspeak.o  score.o  specia.o  act.o     get.o               \
mes.o:    ../common/adv_common

../common/savecm.o:  ../common/savecm.c  ../common/adv_common
	(cd ../common; cc -c $(CFLAGS) savecm.c)

../common/vocab.o:  ../common/vocab.c  ../common/adv_common
	(cd ../common; cc -c $(CFLAGS) vocab.c)


######
clear:
	-rm -f *.b *~
	-rm -f *.o
	-rm -f ../common/*.b ../common/*~
	-rm -f ../common/*.o
	-rm -f $(NAME)
