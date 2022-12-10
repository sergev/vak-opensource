# -std=c++20 -stdlib=libc++ -fmodules -fbuiltin-module-map hello.cc

#CXX             = /usr/local/Cellar/llvm/15.0.3/bin/clang-15 -std=c++20 -fmodules
CXX             = g++ -std=c++20 -fmodules-ts
CXXFLAGS        = -O -g -Wall -Werror
LDFLAGS         = -g

OBJ             = std.o main.o
LIBS            =
HEADERS         = algorithm \
                  array \
                  bitset \
                  chrono \
                  fstream \
                  iomanip \
                  iostream \
                  istream \
                  iterator \
                  limits \
                  list \
                  map \
                  memory \
                  ostream \
                  set \
                  sstream \
                  string \
                  tuple \
                  unordered_map \
                  unordered_set \
                  vector

main:           gcm.cache $(OBJ)
		$(CXX) $(LDFLAGS) $(OBJ) $(LIBS) -o $@

clean:
		rm -rf *.o *.dSYM main gcm.cache

gcm.cache:
		$(CXX) $(CXXFLAGS) -c -x c++-system-header $(HEADERS)
