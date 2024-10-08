#       Makefile     МНОС РЛ 1.2  Антонов В.Г. 27.12.84 от Р.Х.
#
# gcc "port" by vodz 6.2000 vodz@usa.net

CC=     cc
CFLAGS= -O2 -fomit-frame-pointer
OBJS1=  abandon.o attack.o autover.o capture.o check_out.o checkcond.o \
	compkl.o computer.o damage.o damaged.o dcrept.o destruct.o \
	dock.o dumpgame.o dumpme.o dumpssradio.o events.o externs.o \
	getcodi.o getpar.o help.o impulse.o initquad.o kill.o klmove.o

OBJS2=  lose.o lrscan.o main.o move.o nova.o out.o phaser.o play.o ram.o \
	ranf.o rest.o schedule.o score.o setup.o setwarp.o shell.o \
	shield.o snova.o srscan.o systemname.o torped.o utility.o \
	visual.o warp.o win.o cgetc.o

HDRS=   getpar.h trek.h

.c.o    :
	$(CC) $(CFLAGS) -c $*.c

trek:  $(OBJS1) $(OBJS2) $(HDRS)
	@echo " #######   S T A R    T R E K   #######  @VG 27.12.84 "
	$(CC) -s -o trek $(OBJS1) $(OBJS2) -lm

$(OBJS1): $(HDRS)
$(OBJS2): $(HDRS)

clean:
	rm -f trek $(OBJS1) $(OBJS2) core
