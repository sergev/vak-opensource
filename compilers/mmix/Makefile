PROG    = hello hello.mmo echo.mmo primes.mmo

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
	mmix -s $<

trace-chello: hello
	mmix -t2 $<

#
# 'Hello World' in assembler.
#
hello.mmo: hello.mms
	mmixal -l hello.lst $<

run-hello: hello.mmo
	mmix -s $<

trace-hello: hello.mmo
	mmix -t2 $<

#
# /bin/echo in assembler.
#
echo.mmo: echo.mms
	mmixal -l echo.lst $<

run-echo: echo.mmo
	mmix -s $< foo bar

trace-echo: echo.mmo
	mmix -t2 $< foo bar

#
# List of primes in assembler.
#
primes.mmo: primes.mms
	mmixal -l primes.lst $<

run-primes: primes.mmo
	mmix -s $<

trace-primes: primes.mmo
	mmix -t2 $<
