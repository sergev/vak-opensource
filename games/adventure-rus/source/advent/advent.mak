CC = wcc386
CCOPT =
LINK = wlink
LINKOPT =

all : advent.exe

advent.exe : act.obj indobj.obj dark.obj freeze.obj        &
             inpans.obj pct.obj tolower.obj advent.obj     &
             descr.obj get.obj mes.obj rspeak.obj yes.obj  &
             chnloc.obj events.obj getans.obj motion.obj   &
             score.obj conv.obj fatal.obj mscore.obj       &
             specia.obj vocab.obj savecm.obj trim.obj
             $(LINK) $(LINKOPT) @advent.lnk

.c.obj :
        $(CC) $(CCOPT) $*

vocab.obj : $(HEADERS)
	$(CC) $(CCOPT) ..\common\vocab.c

savecm.obj : $(HEADERS)
	$(CC) $(CCOPT) ..\common\savecm.c
