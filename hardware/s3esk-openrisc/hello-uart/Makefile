ifndef CROSS_COMPILE
CROSS_COMPILE = /usr/local/or344/bin/or32-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
NM = $(CROSS_COMPILE)nm
endif

export	CROSS_COMPILE

all: hello.or32

reset.o: reset.S Makefile
	$(CC) -g -c -o $@ $< $(CFLAGS)

hello.o: hello.c Makefile
	$(CC) -g -c -o $@ $< $(CFLAGS)

hello.or32: reset.o hello.o Makefile
	$(LD) -Tram.ld -o $@ reset.o hello.o $(LIBS)
	$(CROSS_COMPILE)objdump -D $@ > hello.dis

System.map: hello.or32
	@$(NM) $< | \
		grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | \
		sort > System.map

#########################################################################
clean:
	find . -type f \
		\( -name 'core' -o -name '*.bak' -o -name '*~' \
		-o -name '*.o'  -o -name '*.a' -o -name '*.tmp' \
		-o -name '*.or32' -o -name '*.bin' -o -name '*.srec' \
		-o -name '*.mem' -o -name '*.img' -o -name '*.out' \
		-o -name '*.aux' -o -name '*.log' \) -print \
		| xargs rm -f
	rm -f System.map

distclean: clean
	find . -type f \
		\( -name .depend -o -name '*.srec' -o -name '*.bin' \
		-o -name '*.pdf' \) \
		-print | xargs rm -f
	rm -f $(OBJS) *.bak tags TAGS
	rm -fr *.*~
