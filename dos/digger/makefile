# Using Borland Turbo C++ 1.01
# (freeware at http://community.borland.com/article/images/21751/tcpp101.zip)
# This makefile was converted to gmake format.
CC		= tcc -1 -v -f- -Z -O -w -k- -DCGA
AS		= a86
C0S		= c:\\tc\\lib\\c0s.obj
LIBS		= c:\\tc\\lib\\cs.lib

digger.exe: 	mkg.exe \
		main.obj digger.obj monster.obj bags.obj sound.obj newsnd.obj \
            	record.obj drawing.obj scores.obj input.obj sprite.obj \
		dospc.obj cgagrafx.obj vgagrafx.obj alpha.obj title.obj \
		ini.obj response.rsp newsnd2.obj
		tlink @response.rsp

digleved.exe: 	digleved.obj vgagrafx.obj alpha.obj cgagrafx.obj \
		eddospc.obj digleved.rsp
		tlink @digleved.rsp

.SUFFIXES:	.asm .obj

.asm.obj:
		$(AS) +o +c +S +P0 $<

.c.obj:
		$(CC) -ms -d -c $<

main.obj: 	main.c def.h hardware.h sound.h sprite.h input.h scores.h \
		drawing.h digger.h monster.h bags.h record.h main.h newsnd.h \
		ini.h

digger.obj: 	digger.c def.h sprite.h input.h hardware.h digger.h drawing.h \
            	main.h sound.h monster.h scores.h bags.h

monster.obj: 	monster.c def.h monster.h main.h sprite.h digger.h drawing.h \
             	bags.h sound.h scores.h

bags.obj: 	bags.c def.h bags.h main.h sprite.h sound.h drawing.h \
		monster.h digger.h scores.h

sound.obj: 	sound.c def.h sound.h hardware.h scores.h

newsnd.obj: 	newsnd.c def.h device.h hardware.h digger.h sound.h

newsnd2.obj: 	newsnd2.c def.h device.h

record.obj: 	record.c def.h record.h hardware.h

drawing.obj: 	drawing.c def.h drawing.h main.h hardware.h sprite.h

scores.obj: 	scores.c def.h scores.h main.h drawing.h hardware.h sound.h \
            	sprite.h input.h digger.h

input.obj: 	input.c def.h main.h sound.h

sprite.obj: 	sprite.c def.h sprite.h hardware.h

ini.obj: 	ini.c def.h

dospc.obj: 	dospc.asm

digleved.obj: 	digleved.c def.h edasm.h

eddospc.obj: 	eddospc.asm

title.obj: 	title.c def.h
		$(CC) -mh -c title.c

alpha.obj: 	alpha.c def.h
		$(CC) -mh -w-sus -c alpha.c

cgagrafx.obj:	cgagrafx.c def.h
		$(CC) -mh -w-sus -c cgagrafx.c

vgagrafx.obj: 	vgagrafx.c def.h
		$(CC) -mh -w-sus -c vgagrafx.c

alpha.c: 	vgatext.spr cgatext.spr mkg.exe
		mkg alpha.c

cgagrafx.c: 	cga.spr mkg.exe
		mkg cgagrafx.c

vgagrafx.c: 	vga.spr mkg.exe
		mkg vgagrafx.c

mkg.exe: 	mkg.obj
		tlink /x /c $(C0S) mkg.obj, mkg.exe,, $(LIBS)
		-del chklist.ms

mkg.obj: 	mkg.c def.h

clean:
		-del *.bak
		-del *.obj
		-del *.map
		-del *.sym
		-del *.exe
		-del vgagrafx.c
		-del cgagrafx.c
		-del alpha.c
		-del chklist.ms
		-del *.tr
		-del *.td
