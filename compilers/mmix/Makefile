PROG    = hello.mmo echo.mmo primes.mmo hello hi echo

all:    $(PROG)
	mmix-size $(PROG)

clean:
	rm -f $(PROG) *.o *.dis *.mmo

#
# 'Hello World' in C.
#
hello:  hello.c
	mmix-gcc -O $< -o $@
	mmix-objdump -D $@ > $@.dis

run-chello: hello
	mmix $<

trace-chello: hello
	mmix -t2 $<

#
# 'Hello World' in GNU assembler.
#
hi:     hi.S
	mmix-gcc -nostdlib $< -o $@
	mmix-objdump -D $@ > $@.dis

run-hi: hi
	mmix $<

trace-hi: hi
	mmix -t2 $<

#
# 'Hello World' in assembler.
#
hello.mmo: hello.mms
	mmixal -l hello.lst $<

run-hello: hello.mmo
	mmix $<

trace-hello: hello.mmo
	mmix -t2 $<

#
# /bin/echo in assembler.
#
echo.mmo: echo.mms
	mmixal -l echo.lst $<

echo:   echo.S
	mmix-gcc -nostdlib $< -o $@
	mmix-objdump -D $@ > $@.dis

run-echo: echo
	mmix $< foo bar

trace-echo: echo
	mmix -t2 $< foo bar

#
# List of primes in assembler.
#
primes.mmo: primes.mms
	mmixal -l primes.lst $<

run-primes: primes.mmo
	mmix $<

trace-primes: primes.mmo
	mmix -t2 $<
