LDLIBS=-lm -lSDL2 -lGLEW -lGL

all: drawing textures

drawing: gltut.o drawing.o

textures: gltut.o textures.o

clean:
	rm -f *.o drawing textures

.PHONY: clean all
