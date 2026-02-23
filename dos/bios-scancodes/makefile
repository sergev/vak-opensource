# For Borland Turbo C++ 3.0
#--------------------------
#
CC      = tcc -ml
CFLAGS	= -G -O -d -Z -Ie:\tcpp101\include
LDFLAGS = -Le:\tcpp101\lib

all:    pckeys.exe

pckeys.exe: pckeys.obj
	$(CC) $(LDFLAGS) -epckeys pckeys.obj

clean:
	if exist *.obj del *.obj
	if exist *.exe del *.exe
	if exist *.bak del *.bak
	if exist *.lst del *.lst
	if exist *.map del *.map
	if exist key.log del key.log

.c.obj:
	$(CC) $(CFLAGS) -c $<
