# For Borland Turbo C++ 3.0
#--------------------------
#
CC      = tcc -I\tc\include -w-inl -w-eff -w-par
CFLAGS	= -ml -G -O -d -Z

all:    rtclock.exe

rtclock.exe: rtclock.obj
	$(CC) $(CFLAGS) rtclock.obj

clean:
	if exist *.obj del *.obj
	if exist *.exe del *.exe
	if exist *.bak del *.bak
	if exist *.lst del *.lst
	if exist *.map del *.map

.c.obj:
	$(CC) $(CFLAGS) -c $<
