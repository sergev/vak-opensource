PROG            = hello echo #quine
MOD             = gm2 -fiso
MODFLAGS	= -O3 -Werror
#LIBS            = -L/usr/lib/gcc/x86_64-linux-gnu/11/m2/m2iso -lm2iso
OBJDUMP         = objdump

all:		$(PROG)

hello:          hello.mod
		$(MOD) $(MLDFLAGS) $(MODFLAGS) $< -o $@ $(LIBS)
		$(OBJDUMP) -d -S $@ > $@.dis

echo:           echo.mod
		$(MOD) $(MLDFLAGS) $(MODFLAGS) $< -o $@ $(LIBS)

quine:          quine.mod
		$(MOD) $(MLDFLAGS) $(MODFLAGS) $< -o $@ $(LIBS)

clean:
		rm -f $(PROG) *.o *~ a.out hello_m2.cpp hello_m2.s
