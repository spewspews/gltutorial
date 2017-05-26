CFLAGS=-Wall -Wpedantic -Wextra -O3 -std=c11 -g
LDLIBS=-lm -lSDL2 -lGLEW -lGL

all: badcube

badcube: ../lib/gltut.a badcube.o

clean:
	rm -f *.o badcube

.PHONY: clean all
