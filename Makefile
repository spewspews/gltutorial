LDLIBS=-lSDL2 -lGLEW -lGL

all: drawing

drawing: gltut.o drawing.o

clean:
	rm -f *.o drawing

.PHONY: clean all
