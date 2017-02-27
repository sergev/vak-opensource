CSC             = mcs
PROG            = hello.exe

all:            $(PROG)

clean:
		rm -f $(PROG)

hello.exe:      hello.cs
		$(CSC) $< -optimize
