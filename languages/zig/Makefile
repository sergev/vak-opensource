PROG    = hello cimport

all:    $(PROG)

clean:
	rm -f $(PROG) *.o

hello:  hello.zig
	zig build-exe $?

cimport: cimport.zig
	zig build-exe $? -I/opt/homebrew/include -L/opt/homebrew/lib -lc -lraylib
