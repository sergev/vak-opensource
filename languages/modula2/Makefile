PROG            = hello
MOD             = /opt/gm2/bin/gm2
MODFLAGS	= -O3 -Werror
#MLDFLAGS	= -L/opt/gm2/lib64
OBJDUMP         = objdump

all:		$(PROG)

hello:          hello.mod
		$(MOD) $(MLDFLAGS) $(MODFLAGS) $< -o $@
#		$(OBJDUMP) -d -S $@ > $@.dis

clean:
		rm -f $(PROG) *.o *~ a.out hello_m2.cpp hello_m2.s
