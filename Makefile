LDLIBS=-lSDL2 -lGLEW -lGL

all: drawing

clean:
	rm -f *.o drawing

.PHONY: clean all
