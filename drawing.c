#include "gltut.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

SDL_Window *screen;
SDL_GLContext glcontext;

GLfloat vertices[] = {
	-0.5,  0.5, 1.0, 0.0, 0.0,	// top left
	 0.5,  0.5, 0.0, 1.0, 0.0,	// top right
	 0.5, -0.5, 0.0, 0.0, 1.0,	// bottom right
	-0.5, -0.5, 1.0, 1.0, 1.0,	// bottom left
};

GLuint elements[] = {
	0, 1, 2,
	2, 3, 0,
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
bindtriangleindices(void)
{
	GLuint ebo;

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	return ebo;
}

double scale = 1.0;

int
uiloop(GLuint shaderprog)
{
	SDL_Event e;
	SDL_Keycode keysym;
	GLint glscale;

	for(;;) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glscale = glGetUniformLocation(shaderprog, "scale");
		glUniform1f(glscale, scale);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(screen);

		while(SDL_PollEvent(&e))
		switch(e.type) {
			case SDL_QUIT:
				return 0;
			case SDL_KEYDOWN:
				keysym = e.key.keysym.sym;
				switch(keysym) {
				case SDLK_q:
				case SDLK_ESCAPE:
					return 0;
				case SDLK_z:
					scale *= 1.3;
					break;
				case SDLK_o:
					scale /= 1.3;
					break;
				}
				break;
		}
		SDL_Delay(100);
	}
}

int
main(void)
{
	GLuint shaderprog, vertexshader, fragmentshader, vao, vbo, ebo;
	GLint position, color;

	initdraw();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	vertexshader = compileshader("flat.vert", GL_VERTEX_SHADER);
	fragmentshader = compileshader("colors.frag", GL_FRAGMENT_SHADER);

	shaderprog = glCreateProgram();
	glAttachShader(shaderprog, vertexshader);
	glAttachShader(shaderprog, fragmentshader);

	glBindFragDataLocation(shaderprog, 0, "outColor");
	glLinkProgram(shaderprog);
	glUseProgram(shaderprog);

	vbo = bindtriangle();
	ebo = bindtriangleindices();

	position = glGetAttribLocation(shaderprog, "position");
	glEnableVertexAttribArray(position);
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);

	color = glGetAttribLocation(shaderprog, "color");
	glEnableVertexAttribArray(color);
	glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));

	uiloop(shaderprog);

	glDeleteProgram(shaderprog);
	glDeleteShader(fragmentshader);
	glDeleteShader(vertexshader);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
	exit(0);
}
