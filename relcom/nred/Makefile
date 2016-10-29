######################################################################
#                                                                    #
#                                           Руднев А.П.              #
#                                       *   ИАЭ им. И.В. Курчатова   #
#                       * **   ***      *   1985                     #
#                       **  * *   *  ****                            #
#                       *     ****  *   *                            #
#                       *     *     *   *                            #
#                       *      ***   ****                            #
#                                                                    #
#                                                                    #
#                                                                    #
#                            ДЕМОС в. I                              #
#                            МНОС РЛ 1.2                             #
######################################################################
#
# $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/Makefile,v 3.1 1986/04/20 23:44:13 alex Exp $
# $Log: Makefile,v $
# Revision 3.1  1986/04/20 23:44:13  alex
# *** empty log message ***
#
# Revision 1.4  86/04/13  21:54:55  alex
#
# DSTDIR=/usr
CFLAGS  = -O -DRUSDIAG  -DFROMTCAP  -DDEBUG
LIFLAGS = -DRUSDIAG -DFROMTCAP -DDEBUG
LDEFS   = r.defs.h  r.local.h
#
# Для МНОС - убрать FROMTCAP, передвинуть ned.mdefs в r.local.h
#
all:    nred

# Вместо r.termc.o можно использовать и -ltermcap

OBJ     = r.cmd.o r.edit.o r.file.o r.gettc.o r.hlam.o r.init.o\
	  r.mac.o r.main.o r.tele.o r.termc.o r.ttyio.o r.wind.o r.mall.o

CSRCS   = r.cmd.c r.edit.c r.file.c r.gettc.c r.hlam.c\
	  r.init.c r.mac.c r.main.c r.tele.c r.termc.c\
	  r.ttyio.c r.wind.c r.mall.c

HSRCS   = r.defs.h r.local.h r.tele.h

SRCS    = $(HSRCS) $(CSRCS)

PUTF    = -l -tRcsdescr -r3
RCSARCHIV = /mnt/cmd/cmd/nred/RCS
RCSPUT  = ci $(PUTF)

nred:   $(OBJ)
	cc $(OBJ) -o nred
# если t>0100000 - убрать -n  и этот комментарий ;c7t10 nred

$(OBJ): $(LDEFS)

r.ttyio.o r.gettc.o: r.tele.h

r.ttyio.o: /usr/include/sgtty.h
r.hlam.o: /usr/include/sys/types.h /usr/include/sys/stat.h

install: nred
	chmod a-t ${DSTDIR}/bin/nred
	-${DSTDIR}/bin/nred </dev/null
	strip nred
	cp nred ${DSTDIR}/bin/nred
	chmod 7111 ${DSTDIR}/bin/nred

clean:
	-rm *.o *.bak nred

LINT:   $(CSRCS) $(HSRCS)
	MSG=r lint $(LIFLAGS) $(CSRCS) > LINT 2>&1

LIST:   $(CSRCS) $(HSRCS)
	grep -n "^[/ ]\*" $(HSRCS) $(CSRCS) > LIST

TAGS:   $(CSRCS)
	ctags -x $(CSRCS) > TAGS

put:    Rcsversion

Rcsversion: $(SRCS) Makefile
	RCS=$(RCSARCHIV) $(RCSPUT) $?
	-date >>Rcsversion
