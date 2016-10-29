PROG            = rapira
CFLAGS		= -m32 -O -Wall -Werror
CXXFLAGS	= $(CFLAGS)
LDFLAGS		= -m32
LIBS            =
OBJ             = lexer.o node.o nodelist.o object.o outcome.o parser.o \
                  rapira.o token.o varmanager.o dividebyzero.o excep.o \
                  invalidassignment.o invalidindex.o invalidinit.o \
                  invalidtype.o lexersyntax.o missingargument.o \
                  negativevalue.o parsersyntax.o add.o and.o call.o \
                  divide.o equal.o exponent.o greateq.o greater.o \
                  intdivide.o length.o less.o lesseq.o multiply.o negate.o \
                  not.o or.o remainder.o select.o slice.o subtract.o \
                  unequal.o integer.o logical.o procedure.o real.o \
                  sequence.o specialfunction.o text.o variable.o \
                  assign.o case.o do.o end.o exit.o extern.o for.o \
                  if.o input.o intern.o output.o repeat.o return.o \
                  selectassign.o sliceassign.o while.o

vpath %.cpp . exceptions operations primitives statements

all:		$(PROG)

clean:
		rm -f $(PROG) *.o *~ a.out

$(PROG):        $(OBJ)
		$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
