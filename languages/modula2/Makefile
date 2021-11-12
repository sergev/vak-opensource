PROG            = hello
MOD             = gm2 -fiso
MODFLAGS	= -O3 -Werror
#LIBS            = -L/usr/lib/gcc/x86_64-linux-gnu/11/m2/m2iso -lm2iso
OBJDUMP         = objdump

all:		$(PROG)

hello:          hello.mod
		$(MOD) $(MLDFLAGS) $(MODFLAGS) $< -o $@ $(LIBS)
		$(OBJDUMP) -d -S $@ > $@.dis

areas:          areas.mod Terminal2.mod
		$(MOD) $(MLDFLAGS) $(MODFLAGS) areas.mod Terminal2.mod -o $@
		$(OBJDUMP) -d -S $@ > $@.dis

clean:
		rm -f $(PROG) *.o *~ a.out hello_m2.cpp hello_m2.s
