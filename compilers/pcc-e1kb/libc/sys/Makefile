SHELL   = /bin/sh
CC      = vcc
INC     = ../../h
INCSYS  = $(INC)/sys
CFLAGS  = -x -I$(INC)

OBJS    =

ALL     = execl.o execle.o execv.o msgsys.o semsys.o shmsys.o

all:    $(OBJS)

clean:
	-rm -f *.o *.b a.out core

msgsys.o: $(INCSYS)/types.h $(INCSYS)/ipc.h $(INCSYS)/msg.h
semsys.o: $(INCSYS)/types.h $(INCSYS)/ipc.h $(INCSYS)/sem.h
shmsys.o: $(INCSYS)/types.h $(INCSYS)/ipc.h $(INCSYS)/shm.h
