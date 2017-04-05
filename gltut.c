#include "gltut.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

SDL_Window *screen;
SDL_GLContext glcontext;

GLuint
compileshader(char *file, GLenum shadertype)
{
	GLint r;
	GLuint shader;
	char buf[512];
	const char *sbuf;
	int fd;
	ssize_t n;

	fd = open(file, O_RDONLY);
	n = read(fd, buf, sizeof(buf)-1);
	if(n <= 0) {
		fprintf(stderr, "Could not read shader %s\n", file);
		abort();
	}
	buf[n] = '\0';

	sbuf = buf;

	shader = glCreateShader(shadertype);
	glShaderSource(shader, 1, &sbuf, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
	if(r != GL_TRUE) {
		fprintf(stderr, "Could not compile shader\n");
		glGetShaderInfoLog(shader, sizeof(buf), NULL, buf);
		fprintf(stderr, "%s:%s", file, buf);
		abort();
	}

	return shader;
}

int
initdraw(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
	                    SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	screen = SDL_CreateWindow("OPENGL!!!!", SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED, 640, 640, SDL_WINDOW_OPENGL);
	glcontext = SDL_GL_CreateContext(screen);
	glewExperimental = GL_TRUE;
	glewInit();

	return 0;
}

void
shutdowndraw(void)
{
        SDL_GL_DeleteContext(glcontext);
        SDL_Quit();
}
