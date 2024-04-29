EXE     = cedit
OBJ     = buffer.o editor.o main.o
LIBS    = -lncurses
CXXFLAGS = -std=c++17 -O -Wall -Werror

all: $(EXE)

clean:
	rm -f $(OBJ) $(EXE)

.PHONY: all clean

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@
###
buffer.o: buffer.cpp buffer.h
editor.o: editor.cpp editor.h buffer.h
main.o: main.cpp editor.h buffer.h
