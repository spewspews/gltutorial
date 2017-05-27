CFLAGS=-Wall -Wpedantic -Wextra -O3 -std=c11 -g
LDLIBS=-lm -lSDL2 -lGLEW -lGL

all: cube

cube: ../lib/gltut.a cube.o

clean:
	rm -f *.o cube

.PHONY: clean all
