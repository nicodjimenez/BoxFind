CXX = clang++
CFLAGS = -stdlib=libc++ -std=c++11 -O2 -Wall

all: test

clean: 
	rm test

test : test.cpp boxKdTree.hpp
	$(CXX) $(CFLAGS) -o test.bin $<
	
	
