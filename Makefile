CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2

OBJECTS = main.o

HEADERS = CLI11.hpp graph.hpp log.hpp

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJECTS): $(HEADERS)

clean:
	rm -f $(OBJECTS) main

PHONY: .clean
