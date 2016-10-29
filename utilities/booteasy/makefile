all:		boot.bin boot.dis

clean:
		rm -f *~ boot.bin boot.dis boot.lst

boot.bin:	boot.s
		nasm -a -f bin -o boot.bin -l boot.lst boot.s

boot.dis:	boot.bin
		ndisasm -a -k 0x12f,0xd1 boot.bin > boot.dis

boot17.dis:
		ndisasm -a -k 0x12f,0xd1 boot17.bin > boot17.dis
