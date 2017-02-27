NIMC            = nim
PROG            = hello

all:            $(PROG)

clean:
		rm -f $(PROG)
		rm -rf nimcache

hello:          hello.nim
		$(NIMC) compile -d:release $<
