LDLIBS=-lm -lSDL2 -lGLEW -lGL

all: drawing textures texturesex1 reflection

drawing: gltut.o drawing.o

textures: gltut.o image.o textures.o

texturesex1: gltut.o image.o texturesex1.o

reflection: gltut.o image.o reflection.o

clean:
	rm -f *.o drawing textures texturesex1 

.PHONY: clean all
