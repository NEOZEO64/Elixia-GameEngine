CXX       := /usr/bin/g++
CXX_FLAGS := -std=c++17 -fdiagnostics-color=always -g -Wall -Wpedantic `sdl2-config --libs --cflags`
LIBRARIES := -lfmt -lSDL2_image -lSDL2_ttf $(shell sdl2-config --cflags)

SRC     := src
INCLUDE := include

all: clean main.out

main.out: main.cpp $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

run: all
	./main.out

clean:
	rm -f *.out
