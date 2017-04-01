LDLIBS=-lSDL2 -lGLEW

all: drawing

clean:
	rm -f *.o drawing

.PHONY: clean all
