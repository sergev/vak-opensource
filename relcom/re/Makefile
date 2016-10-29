######################################################################
#                                                                    #
#                                                                    #
#                                       *                            #
#                       * **   ***      *   1985                     #
#                       **  * *   *  ****                            #
#                       *     ****  *   *                            #
#                       *     *     *   *                            #
#                       *      ***   ****                            #
#                                                                    #
#                                                                    #
#                                                                    #
#                            ДEМОС в. I                              #
#                            МНОС РЛ 1.2                             #
#                            ДEМОС/EС в. 1.
######################################################################
RED=lred
# install - установка в LOCAL
# rinstall - установка на удаленную машину
# binstall - установка в BIN основных компонент
# rbinstall- то же на удаленную ЭВМ
# lred - red for System-V
# ared - red for UTEC / DEMOS
# ered - редактор для EС
# nred - редактор для СМ
# ired - редактор для Электроники-79
# tred - отладочная версия для эмуляции EС на СМ.
# ured - Редактор для UTEC
# dred - Редактор для ULTRIX-32 / BSD 4.3 /
#       $Header: Makefile,v 4.20 90/05/22 23:22:43 alex Exp $

all: $(RED) rpl
#
#  OBJ и LOCAL переопределяются при вызове make в случае ered, tred
#
LOCAL=L.locl.h
OBJ=$(OBJS) $(OBJC)

DSTDIR=/usr/local
DSTCPU=x8
DSTNAME=re
RED=edit
CFL=-O
LDFLAGS=
MAKE=make
# CFL=-O -DRUSDIAG -DDEBUG -DFROMTCAP
#
# Для МНОС - убрать FROMTCAP, установить LOCAL=M.locl.h
#
PUT=-f -r4.20 -m"First rev. red 4.2"
PUTF=-q -u -r4 $(PUT)
GETF=-r4
RCSARCHIV=
RCSPUT=ci $(PUTF)

# Вместо S.termc.o можно использовать и -ltermcap
#
# OBJC - общие об'ектные файлы
# OBJEC - общие для ered и tred
# OBJEE - специфические файлы для ered
# OBJEQ - специфические для tred
# OBJS  - специфические для СМ и Электроники файлы

OBJC=r.pars.o r.cmd.o r.edit.o r.file.o r.hlam.o r.init.o\
   r.lop.o r.mac.o r.main.o r.rdf.o r.var.o r.tele.o  r.tabs.o r.wind.o r.mall.o

OBJEC=E.param.o E.ttyio.o
OBJEE=E.qsrt.o E.tubecc.o
OBJEQ=E.qsemul.o
OBJS=S.ttyio.o S.gettc.o S.termc.o

CSRCS=r.pars.c r.cmd.c r.edit.c r.file.c r.hlam.c\
    r.init.c  r.lop.c r.mac.c r.main.c r.tele.c  r.tabs.c\
    r.wind.c r.mall.c r.var.c r.rdf.c\
    S.ttyio.c S.termc.c S.gettc.c

ESRCS=E.param.c E.ttyio.c E.qsrt.c E.tubecc.s E.qsemul.c

HSRCS=r.defs.h  S.tele.h V.locl.h A.locl.h U.locl.h
EHDRS=E.qsdefs.h  E.locl.h S.locl.h S2.locl.h M.locl.h E.7920.h E.r7920.h
SRCS=$(HSRCS) $(CSRCS) rpl.c Makefile Newvers


ered: Ered *.[chs] Makefile
	make -k LDFLAGS=-n LOCAL=E.locl.h OBJ="$(OBJC) $(OBJEC) $(OBJEE)" edit;rm -f ered;ln edit ered

tred: Tred *.[chs] Makefile
	make -k LOCAL=E.locl.h OBJ="$(OBJC) $(OBJEC) $(OBJEQ)" edit;rm -f tred;ln edit tred

nred: Nred *.[chs] Makefile  Newvers
	make -k CFL="-O -DRUSDIAG" LOCAL=V.locl.h OBJ="$(OBJC) $(OBJS)" edit;c7t10 edit;rm -f nred;ln edit nred

ired: Nred *.[chs] Makefile  Newvers
	make -k CFL="-O -DALLDIAG -DRED_CYRILL" LDFLGS=-i LOCAL=V.locl.h OBJ="$(OBJC) $(OBJS)" edit;rm -f ired;ln edit ired

sred: Sred *.[chs] Makefile
	make -k LOCAL=S.locl.h OBJ="$(OBJC) $(OBJS)" edit;rm -f sred;ln edit sred

vred: Vred *.[chs] Makefile  Newvers
	make -k LOCAL=V.locl.h OBJ="$(OBJC) $(OBJS)" CFL="-O -DALLDIAG" edit;rm -f vred;ln edit vred

dred: Dred *.[chs] Makefile  Newvers
	make -k LOCAL=D.locl.h OBJ="$(OBJC) $(OBJS)" CFL="-O -DALLDIAG" edit;rm -f dred;ln edit dred

xred: Xred *.[chs] Makefile  Newvers
	make -k LOCAL=L.locl.h OBJ="$(OBJC) $(OBJS)" LDFLAGS="-M2s -F 6000 -i" CFL="-M2s -O" edit;rm -f xred;ln edit xred

lred: Lred *.[chs] Makefile  Newvers
	make -k LOCAL=L.locl.h OBJ="$(OBJC) $(OBJS)" CFL="-DALLDIAG" edit;rm -f lred;ln edit lred

ared: Ared *.[chs] Makefile Newvers
	make -k LOCAL=A.locl.h OBJ="$(OBJC) $(OBJS)" CFL="-O -DALLDIAG" edit;rm -f ared;ln edit ared

ured: Ured *.[chs] Makefile  Newvers
	make -k LOCAL=U.locl.h OBJ="$(OBJS) $(OBJC)" CFL="-O" edit;rm -f ured;ln edit ured


clean:
	rm -rf *.o *.b *.bak ?red edit  rpl

Ered:
	  rm -rf *.o *.bak ?red edit
	  @echo >Ered
	  @echo "Справочник подготовлен для сборки версии для EС"

Xred:
	  rm -rf *.o *.bak ?red edit
	  @echo >Xred
	  @echo "Справочник подготовлен для сборки версии для Xenix-286"

Dred:
	  rm -rf *.o *.bak ?red edit
	  @echo >Dred
	  @echo "Справочник подготовлен для сборки версии для ULTRIX"

Ared:
	  rm -rf *.o *.bak ?red edit
	  @echo >Ared
	  @echo "Справочник подготовлен для сборки версии для UTEC/Anshukow"

Lred:
	 rm -rf *.o *.bak ?red edit
	 @echo >Lred
	 @echo "Справочник подготовлен для сборки версии для SYSTEM-V"

Nred:
	 rm -rf *.o *.bak ?red edit
	 @echo >Nred
	 @echo "Справочник подготовлен для сборки версии для SM"

Sred:
	 rm -rf *.o *.bak ?red edit
	 @echo >Sred
	 @echo "Справочник подготовлен для сборки версии для SM ДЕМОС 2"

Vred:
	 rm -rf *.o *.bak ?red edit
	 @echo >Vred
	 @echo "Справочник подготовлен для сборки версии для ДЕМОС/V"

Tred:
	 rm -rf *.o *.bak ?red edit
	 @echo >Tred
	 @echo "Справочник подготовлен для сборки эмулятора  EС"


Ured:
	 rm -rf *.o *.bak ?red edit
	 @echo >Ured
	 @echo "Справочник подготовлен для сборки версии для ДЕМОС/V"


#
#  Общая схема сборки редактора
#

CFLAGS=$(CFL) -DLOCL="\"$(LOCAL)\""
LIFLAGS=$(CFL) -DLOCL="\"$(LOCAL)\""
LDEFS=r.defs.h  $(LOCAL)

$(OBJ):$(LDEFS)
$(OBJEQ) $(OBJEE):E.qsdefs.h  /usr/include/sgtty.h
$(OBJEC): E.7920.h E.r7920.h

edit: Makefile $(OBJ) Vers.o
	cc -o edit $(LDFLAGS) $(OBJ) Vers.o
Vers.o: $(OBJ)  Newvers
	csh -f Newvers
	cc $(CFLAGS) -c Vers.c

rpl: rpl.c
	cc -o rpl -O rpl.c


E.ttyio.o : E.7920.h E.r7920.h
S.ttyio.o S.gettc.o: S.tele.h
E.ttyio.o S.ttyio.o: /usr/include/sgtty.h
r.hlam.o: /usr/include/sys/types.h /usr/include/sys/stat.h
E.qsemul.o: E.qsdefs.h
E.tubecc.o: E.tubecc.s
	as -u -o E.tubecc.o E.tubecc.s

install: $(RED) edit lib.red rpl
	-chmod a-t ${DSTDIR}/bin/$(DSTNAME)
	cp rpl $(DSTDIR)/bin/rpl
	-strip $(DSTDIR)/bin/rpl
	-strip $(RED)
	cp $(RED) ${DSTDIR}/bin/$(DSTNAME)
	chmod 7111 ${DSTDIR}/bin/$(DSTNAME)
	cd lib.red;$(MAKE) DSTDIR=$(DSTDIR) install

rinstall: edit red.lib/re.std red.lib/help.* rpl
	-rsh ${DSTCPU} chmod a-t ${DSTDIR}/bin/$(DSTNAME)
	strip $(RED)
	rcp $(RED) ${DSTCPU}:${DSTDIR}/bin/$(DSTNAME)
	rsh ${DSTCPU} chmod 7111 ${DSTDIR}/bin/$(DSTNAME)
	cd lib.red;$(MAKE) DSTDIR=$(DSTDIR) DSTCPU=$(DSTCPU) rinstall
	rcp rpl ${DSTCPU}:/bin/rpl
	-rsh ${DSTCPU} strip /bin/rpl

binstall:       install
	-mv /bin/$(DSTNAME) /bin/$(DSTNAME).old
	cp $(DSTDIR)/bin/$(DSTNAME) $(DSTDIR)/bin/rpl /bin

rbinstall:      rinstall
	-rsh $(DSTCPU) mv /bin/$(DSTNAME) /bin/$(DSTNAME).old
	rsh $(DSTCPU) cp $(DSTDIR)/bin/$(DSTNAME) $(DSTDIR)/bin/rpl /bin

clean:

LINT: $(CSRCS) $(HSRCS)
	MSG=r lint $(LIFLAGS) $(CSRCS) > LINT 2>&1

LIST: $(CSRCS) $(HSRCS)
	grep -n "^[/ ]\*" $(HSRCS) $(CSRCS) > LIST

TAGS: $(CSRCS)
	ctags -x $(CSRCS) > TAGS


put: Rcsversion

get: $(SRCS)
.DEFAULT:
	co $(GETF) $@

Rcsversion: $(SRCS)
	$(RCSPUT) $?
	-date >>Rcsversion

#       $Log:	Makefile,v $
# Revision 4.20  90/05/22  23:22:43  alex
# First rev. red 4.2
# 
# Revision 4.10  90/02/05  19:51:46  alex
# Base revision 4.1
# 
# Revision 4.10  90/02/05  19:49:33  alex
# Base revision 4.1
# 
# Revision 4.2  88/04/04  22:18:47  alex
# 
# Revision 4.1  88/03/31  22:08:18  alex
# Версия 4.1 - обкатка на UTEC, СМ
# 
# Revision 3.1.2.4  87/06/23  20:53:54  alex
# Automatic_revisions
# 
# Revision 3.1.2.3  87/06/23  19:58:44  alex
# Внесенна Newvers.
# 
# Revision 3.1.2.2  87/06/23  18:58:35  alex
# wYNESENA PEREMENNAQ lread1 I \TO OTLAVENO
# 
# Revision 3.1.2.1  87/06/19  17:03:26  alex
# Start revision for red/4
# 
# Revision 3.9  87/06/12  17:22:19  alex
# Включена версия dred для ULTRIX и ired для машин с SEPID.
# 
# Revision 3.9  87/04/01  19:13:04  alex
# Revision for Utec: Cs, Ce, Ct, hR
# 
# Revision 3.8  86/10/14  23:04:44  alex
# Исправлены мелкие ошибки, введено .DEFAULT.
# 
# Revision 3.7  86/10/14  21:03:06  alex
# Исправлена ошибка в S.ttyio.c -> S.ttyio.o
# 
# Revision 3.6  86/09/19  20:02:00  alex
# Версия для СМ-1700
# 
# Revision 3.5  86/08/04  20:56:20  alex
# Bepqh dk LMNQ/DELNQ 2
# 
# Revision 3.4  86/07/24  23:39:19  alex
# Мелочи.
#
# Revision 3.3  86/07/24  21:09:23  alex
# Мелочи
#
# Revision 3.2  86/07/24  19:09:16  alex
# Об'единены версии текстов для ЕС и СМ
#
# Revision 3.1.1.11  86/07/15  23:52:08  alex
# *** empty log message ***
#
# Revision 3.1.1.10  86/07/15  22:15:48  alex
# RED 3.3/EC.
#
# Revision 3.1.1.9  86/06/25  19:43:20  alex
# Введено автоматическое смещение экрана
#
# Revision 3.1.1.8  86/06/19  00:12:26  alex
# Отлажен на модели повтор сеанса
#
# Revision 3.1.1.7  86/06/17  19:28:26  alex
# Правки.
#
# Revision 3.1.1.6  86/06/16  22:29:56  alex
# Первая версия с повтором сеанса (пока грязная)
#
# Revision 3.1.1.5  86/06/16  22:21:50  alex
# Первая версия с повтором сеанса (пока грязная)
#
# Revision 3.1.1.4  86/06/16  21:55:32  alex
# Отлажено на EС без режима восстановления и без макро
#
# Revision 3.1.1.3  86/06/06  00:15:04  alex
# Введена установка ключей
#
# Revision 3.1.1.2  86/06/05  18:57:18  alex
# Отлажено_на_модели
#
# Revision 3.1.1.1  86/06/05  00:08:06  alex
# Отлажено_на_модели
#
# Revision 3.1  86/04/20  23:44:13  alex
# Базовая версия для EС.
#
#
# Revision 1.4  86/04/13  21:54:55  alex
#
