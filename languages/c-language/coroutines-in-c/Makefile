# Build and run the C++20 coroutine demos.
#   fib    — co_yield generator demo                    (make run)
#   task   — co_await composition demo                  (make run-task)
#   taskc  — same demo, but add() is written in C       (make run-taskc)
#   taskcc — same demo, but add() AND compute() are C   (make run-taskcc)

CXX      := c++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2
CC       := cc
CFLAGS   := -std=c11 -Wall -Wextra -O2

FIB_OBJS   := main.o fib.o
TASK_OBJS  := task_main.o compute.o add.o
# taskc reuses the C++ objects but swaps the C add() for the C++ one.
TASKC_OBJS := task_main.o compute.o cadd.o
# taskcc goes all the way: both compute() and add() are the C versions.
TASKCC_OBJS := task_main.o computec.o cadd.o

.PHONY: all run run-task run-taskc run-taskcc clean

all: fib task taskc taskcc

# --- co_yield generator demo -------------------------------------------------

fib: $(FIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(FIB_OBJS)

main.o: main.cpp fib.hpp generator.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ main.cpp

fib.o: fib.cpp fib.hpp generator.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ fib.cpp

# Build (if needed) and run. Pass N on the command line, e.g. `make run N=100`.
run: fib
	./fib $(N)

# --- co_await composition demo -----------------------------------------------

task: $(TASK_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(TASK_OBJS)

task_main.o: task_main.cpp compute.hpp task.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ task_main.cpp

compute.o: compute.cpp compute.hpp add.hpp task.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ compute.cpp

add.o: add.cpp add.hpp task.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ add.cpp

run-task: task
	./task

# --- co_await demo with add() implemented in plain C -------------------------

# Links the unchanged C++ objects against a C-defined add() (cadd.o). Uses the
# c++ driver because the C++ runtime is still needed. add.o is deliberately NOT
# in the link line — cadd.o provides the same _Z3addii symbol.
taskc: $(TASKC_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(TASKC_OBJS)

cadd.o: cadd.c
	$(CC) $(CFLAGS) -c -o $@ cadd.c

run-taskc: taskc
	./taskc

# --- co_await demo with BOTH compute() and add() in plain C ------------------

# Both coroutines are now C: computec.o (compute.c) + cadd.o (cadd.c). Only the
# driver (task_main) and Task<T> plumbing are still C++. Note compute.c compiles
# to computec.o so it never clashes with compute.o from compute.cpp.
taskcc: $(TASKCC_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(TASKCC_OBJS)

computec.o: compute.c
	$(CC) $(CFLAGS) -c -o $@ compute.c

run-taskcc: taskcc
	./taskcc

clean:
	rm -f fib task taskc taskcc $(FIB_OBJS) $(TASK_OBJS) cadd.o computec.o
