#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>

SDL_Window *screen;
SDL_GLContext glcontext;

float vertices[] = {
	0.0, 0.5,
	0.5, -0.5,
	-0.5, -0.5,
};

GLuint
bindtriangle(void)
{
	GLuint vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	return vbo;
}

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

int uiloop(GLint tricolor)
{
	SDL_Event e;
	SDL_Keycode keysym;
	struct timespec tsstart, tsnow;
	double tdiff;

	clock_gettime(CLOCK_MONOTONIC, &tsstart);
	for(;;) {
		clock_gettime(CLOCK_MONOTONIC, &tsnow);
		tdiff = tsnow.tv_nsec - tsstart.tv_nsec;
		glUniform3f(tricolor, (sin(fmod(tdiff*4.0, M_PI*2)) + 1.0) / 2.0, 0.0, 0.0);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(screen);

		while(SDL_PollEvent(&e))
		switch(e.type) {
			case SDL_QUIT:
				return 0;
			case SDL_KEYDOWN:
				keysym = e.key.keysym.sym;
				if(keysym == SDLK_q || keysym == SDLK_ESCAPE)
					return 0;
		}
		SDL_Delay(100);
	}
}

int
main(void)
{
	GLuint shaderprog, vertexshader, fragmentshader, vao, vbo;
	GLint position, tricolor;

	initdraw();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	vertexshader = compileshader("flat.vert", GL_VERTEX_SHADER);
	fragmentshader = compileshader("white.frag", GL_FRAGMENT_SHADER);

	shaderprog = glCreateProgram();
	glAttachShader(shaderprog, vertexshader);
	glAttachShader(shaderprog, fragmentshader);

	glLinkProgram(shaderprog);
	glUseProgram(shaderprog);

	vbo = bindtriangle();

	position = glGetAttribLocation(shaderprog, "position");
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position);

	tricolor = glGetUniformLocation(shaderprog, "tricolor");
	uiloop(tricolor);

	glDeleteProgram(shaderprog);
	glDeleteShader(fragmentshader);
	glDeleteShader(vertexshader);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
	exit(0);
}
