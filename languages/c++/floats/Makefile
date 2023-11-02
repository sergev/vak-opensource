PROG    = print_floats \
          print_bfloat16 \
          print_minifloat8 \
          float_epsilons \
          bfloat16_epsilons
CFLAGS  = -O3 -std=c++17 -Wall

all:    $(PROG)

print_floats: print_floats.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

float_epsilons: float_epsilons.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

print_bfloat16: print_bfloat16.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

print_minifloat8: print_minifloat8.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@

bfloat16_epsilons: bfloat16_epsilons.cpp
	$(CXX) $(LDFLAGS) $(CFLAGS) $< -o $@


clean:
	rm -f $(PROG) *.o
