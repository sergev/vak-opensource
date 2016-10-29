UNAME_S         := $(shell uname -s)

# -std=c++11 -Wall -march=native
ifeq ($(UNAME_S),Linux)
CXX             = g++
CC              = gcc -std=gnu99
CFLAGS          = -O -g -I..
CXXFLAGS        = -O -g -I..
#CPPFLAGS        = -O -I.. -DCL_USE_DEPRECATED_OPENCL_1_1_APIS
LDFLAGS         = -g
#LDLIBS          = -lPVROCL
LDLIBS          = -lOpenCL
#LDLIBS          = -lOpenCL
endif

ifeq ($(UNAME_S),Darwin)
CXX             = clang++
CFLAGS          = -O -g -I..
CXXFLAGS        = -O -g -I..
LDFLAGS         = -g
LDLIBS          = -framework OpenCL
endif

EXEC            = cldemo #device_query

all:            $(EXEC)

device_query:   device_query.o
		$(CXX) $(LDFLAGS) -o $@ device_query.o $(LDLIBS)

cldemo:         cldemo.o
		$(CC) $(LDFLAGS) -o $@ cldemo.o $(LDLIBS)

%.o:            %.cpp
		$(CXX) $(CXXFLAGS) -c $<

%.o:            %.c
		$(CC) $(CFLAGS) -c $<

clean:
		rm -f *.o $(EXEC)
