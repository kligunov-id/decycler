CXX = g++
CXXFLAGS = -std=c++23 -Wall -O2

OBJECTS = main.o generator.0

HEADERS = CLI11.hpp graph.hpp log.hpp

TARGETS = decycle generate

all: $(TARGETS)

decycle: main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

generate: generator.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJECTS): $(HEADERS)

clean:
	rm -f $(OBJECTS) $(TARGETS)

.PHONY: clean all
