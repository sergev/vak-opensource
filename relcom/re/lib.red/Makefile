#
#
# Библиотека редактора red - справочные файлы и таблицы
# Всем этим компонентам место в /usr/local/lib/red
# termcap -> /usr/local/etc
#
# Здесь содержатся файлы трех типов
# 1. Таблицы кодировки (транслируются и запускаются).
# 2. Файлы инициализации редактора
# 3. Справочные файлы
# Кроме того, содержится файл termcap.
#
ALL =$(CYRTABS) $(INITABS) $(HELP)
HELP=help.*
INITABS=init.dflt
CYRTABS=Wyse75 VT200 PVT100 QWERTY8
DSTDIR=/usr/local
DSTCPU=cpux8

all:  $(ALL)


Wyse75:  Wyse75.c
	cc Wyse75.c
	a.out > Wyse75
	rm -f a.out

PVT100:PVT100.c
	cc PVT100.c
	a.out > PVT100
	rm -f a.out

QWERTY8:  QWERTY8.c
	cc QWERTY8.c
	a.out > QWERTY8
	rm -f a.out


VT200:  VT200.c
	cc VT200.c
	a.out > VT200
	rm -f a.out

install: all
	-mkdir $(DSTDIR)/lib/red
	cp $(ALL) $(DSTDIR)/lib/red
	set -x;for i in $(INITABS);do mv $(DSTDIR)/lib/red/$$i $(DSTDIR)/lib/red/$${i}src;sh comm_remove < $$i > $(DSTDIR)/lib/red/$$i;done
	-mkdir $(DSTDIR)/lib/fonts
	cp fontVT200 $(DSTDIR)/lib/fonts/vt200
	echo "Join 'termcap' and you '/usr/local/etc/termcap' OR '/etc/termcap'"

rinstall: all
	-rsh $(DSTCPU) mkdir $(DSTDIR)/lib/red
	rcp $(ALL) $(DSTCPU):$(DSTDIR)/lib/red
	-rsh $(DSTCPU) mkdir $(DSTDIR)/lib/fonts
	rcp fontVT200 $(DSTCPU):$(DSTDIR)/lib/fonts/vt200
	echo "Join 'termcap' and you '$(DSTCPU):/usr/local/etc/termcap' OR '$(DSTCPU):/etc/termcap'"
