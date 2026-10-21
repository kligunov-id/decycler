CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2

OBJECTS = main.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJECTS): CLI11.hpp

clean:
	rm -f $(OBJECTS) main

PHONY: .clean
