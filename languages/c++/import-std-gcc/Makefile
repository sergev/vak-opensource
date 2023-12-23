CXX             = g++-13 -std=c++20 -fmodules-ts
CXXFLAGS        = -O -g -Wall -Werror
LDFLAGS         = -g

OBJ             = std.o main.o
LIBS            =
HEADERS         = iostream
# algorithm array bitset chrono fstream iomanip istream iterator limits list map memory ostream set sstream string tuple unordered_map unordered_set vector

all:            main

.NOTPARALLEL:
main:           gcm.cache $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

clean:
		rm -rf *.o *.dSYM main gcm.cache

gcm.cache:
		$(CXX) $(CXXFLAGS) -c -x c++-system-header $(HEADERS)
