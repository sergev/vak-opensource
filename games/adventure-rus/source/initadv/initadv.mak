CC = wcc386
CCOPT =
LINK = wlink
LINKOPT =

all : initadv.exe

initadv.exe : fatal.obj getlin.obj getobj.obj iactio.obj icave.obj iclass.obj &
              ievent.obj imessa.obj initadv.obj iobjec.obj ivocab.obj         &
              putcnd.obj putmes.obj screen.obj stat.obj vocab.obj savecm.obj
              $(LINK) $(LINKOPT) @initadv.lnk

.c.obj :
        $(CC) $(CCOPT) $*

vocab.obj : $(HEADERS)
	$(CC) $(CCOPT) ..\common\vocab.c

savecm.obj : $(HEADERS)
	$(CC) $(CCOPT) ..\common\savecm.c
