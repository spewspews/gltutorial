LDLIBS=-lm -lSDL2 -lGLEW -lGL

all: drawing textures texturesex1 reflection water

glerrors.h: glerrors
	./generrorstrings.awk $< > $@

gltut.o: glerrors.h

lib/gltut.a: gltut.o image.o
	$(AR) rcs $@ $^ 

drawing: gltut.o drawing.o

textures: lib/gltut.a textures.o

texturesex1: lib/gltut.a texturesex1.o

reflection: lib/gltut.a reflection.o

water: lib/gltut.a water.o

clean:
	rm -f *.o drawing textures texturesex1 reflection water glerrors.h

.PHONY: clean all
