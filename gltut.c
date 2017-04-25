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

#include "glerrors.h"

void
glerrchk(char *fmt, ...)
{
	static char buf[1024];
	GLenum err;
	va_list va;
	int w;

	if((err = glGetError()) == GL_NO_ERROR)
		return;

	va_start(va, fmt);
	w = vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);
	snprintf(buf+w, sizeof(buf)-w, ": %s\n", glerrors[err]);
	fputs(buf, stderr);
	exit(1);
}

GLint
getuni(GLuint shaderprog, char *unistr)
{
	GLint uni;

	uni = glGetUniformLocation(shaderprog, unistr);
	if(uni == -1) {
		fprintf(stderr, "Could not get uniform %s\n", unistr);
		exit(1);
	}
	return uni;
}

void
setuni1i(GLuint shaderprog, char *unistr, int i)
{
	GLint uni;

	uni = getuni(shaderprog, unistr);
	glUniform1i(uni, i);
}

GLuint
compileshader(char *file, GLenum shadertype)
{
	GLint r;
	GLuint shader;
	char buf[8192];
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
initdraw(char *title)
{
	GLenum err;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
	                    SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	screen = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED, 640, 640, SDL_WINDOW_OPENGL);
	glcontext = SDL_GL_CreateContext(screen);
	glewExperimental = GL_TRUE;
	glewInit();
	err = glGetError();
	if(err != GL_NO_ERROR && err != GL_INVALID_ENUM) {
		fprintf(stderr, "glewInit failed: %d\n", err);
		exit(1);
	}
	glerrchk("%s: glewInit failed", __func__);

	return 0;
}

void
shutdowndraw(void)
{
        SDL_GL_DeleteContext(glcontext);
        SDL_Quit();
}
