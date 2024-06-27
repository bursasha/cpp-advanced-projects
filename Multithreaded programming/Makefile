# C++ compiler and linker to use for building the project
CXX=/opt/homebrew/Cellar/gcc/12.2.0/bin/aarch64-apple-darwin22-g++-12
LD=/opt/homebrew/Cellar/gcc/12.2.0/bin/aarch64-apple-darwin22-g++-12

# Archive tool for creating static libraries
AR=ar

# Compiler flags
CXXFLAGS=-std=c++17 -Wall -pedantic -g

# Shell to use for executing make commands
SHELL:=/bin/bash

# Machine-specific directory name based on architecture and OS type
MACHINE=$(shell uname -m)-$(shell echo $$OSTYPE)

# Default target to build when running `make` without arguments
all: test

# Generate dependencies for the Makefile
deps:
	g++ -MM *.cpp > Makefile.d

# Link the object files into the final executable named `test`
test: solution.o sample_tester.o
	$(LD) $(CXXFLAGS) -o $@ $^ -L./$(MACHINE) -lprogtest_solver -lpthread

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Create a static library from the object file progtest_solver.o
lib: progtest_solver.o
	mkdir -p $(MACHINE)
	$(AR) cfr $(MACHINE)/libprogtest_solver.a $^

# Clean the build directory by removing all generated files
clean:
	rm -f *.o test *~ core sample.tgz Makefile.d

# Clean the build directory and create a tarball package of the project
pack: clean
	rm -f sample.tgz
	tar zcf sample.tgz --exclude progtest_solver.cpp --exclude Makefile.mingw *

# Include the generated dependencies
-include Makefile.d
