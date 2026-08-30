CXX = g++

CXXFLAGS = -Wall -Werror -std=c++20 -O3 -Iinclude
LIBS = -lSDL2 -lSDL2_ttf -lm

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

TARGET = main

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LIBS)

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	rm -rf build
