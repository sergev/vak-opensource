
all:		ipl.com boot.rom

ipl.com:	ipl8086.s
		-@rm -rf ipl.com
		nasm -DCOM ipl8086.s -o ipl.com

boot.rom:	ipl8086.s makerom.pl disrom.pl
		-@rm -rf boot.rom
		nasm ipl8086.s -o boot.rom
		./makerom.pl boot.rom
		./disrom.pl boot.rom

clean:
		rm -rf boot.rom ipl.com *~
