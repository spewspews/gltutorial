#include "gltut.h"
#include "stb_image.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdint.h>
 
typedef struct Vertex Vertex;
struct Vertex {
	struct {
		float x, y;
	} pos;
	struct {
		float r, g, b;
	} color;
	struct {
		float s, t;
	} texpos;
};

Vertex vertices[] = {
	{{-0.5,  0.5}, {1.0, 0.0, 0.0}, {0.0, 0.0}},     // top left
	{{ 0.5,  0.5}, {0.0, 1.0, 0.0}, {1.0, 0.0}},     // top right
	{{ 0.5, -0.5}, {0.0, 0.0, 1.0}, {1.0, 1.0}},     // bottom right
	{{-0.5, -0.5}, {1.0, 1.0, 1.0}, {0.0, 1.0}},     // bottom left
};

GLuint elements[] = {
	0, 1, 2,
	2, 3, 0,
};

void
bindtexture(char *file, GLuint tex, int active)
{
	int x, y;
	uint8_t *image;

	glActiveTexture(GL_TEXTURE0 + active);
	glBindTexture(GL_TEXTURE_2D, tex);

	image = stbi_load(file, &x, &y, NULL, STBI_rgb);
	if(image == NULL) {
		fprintf(stderr, "Could not load glenda\n");
		abort();
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	stbi_image_free(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
}

void
bindtriangle(GLuint vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void
bindindices(GLuint ebo)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

#define pi 3.14159265358979323846

int
uiloop(GLint glwater)
{
	SDL_Event e;
	SDL_Keycode keysym;
	float water;
	int i;
	enum {
		BLENDMAX = 50
	};

	i = 0;
	for(;;) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		water = (float)i*pi*2 / BLENDMAX;
		if(++i == BLENDMAX)
			i = 0;
		glUniform1f(glwater, water);

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
				}
				break;
		}
		SDL_Delay(20);
	}
}

int
main(void)
{
	GLuint shaderprog, vertexshader, fragmentshader,
		vao, glbo[2], tex;
	GLint position, color, texpos, glendatex;

	initdraw("water");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	vertexshader = compileshader("watershaders/flat.vert", GL_VERTEX_SHADER);
	fragmentshader = compileshader("watershaders/colors.frag", GL_FRAGMENT_SHADER);

	shaderprog = glCreateProgram();
	glAttachShader(shaderprog, vertexshader);
	glAttachShader(shaderprog, fragmentshader);

	glBindFragDataLocation(shaderprog, 0, "outColor");
	glLinkProgram(shaderprog);
	glUseProgram(shaderprog);

	glGenBuffers(2, glbo);
	bindtriangle(glbo[0]);
	bindindices(glbo[1]);

	glGenTextures(1, &tex);
	bindtexture("glenda.gif", tex, 0);

	glendatex = glGetUniformLocation(shaderprog, "glendatex");
	glUniform1i(glendatex, 0);

	position = glGetAttribLocation(shaderprog, "position");
	glEnableVertexAttribArray(position);
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

	color = glGetAttribLocation(shaderprog, "color");
	glEnableVertexAttribArray(color);
	glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	texpos = glGetAttribLocation(shaderprog, "texpos");
	glEnableVertexAttribArray(texpos);
	glVertexAttribPointer(texpos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texpos));

	uiloop(glGetUniformLocation(shaderprog, "water"));

	glDeleteProgram(shaderprog);
	glDeleteShader(fragmentshader);
	glDeleteShader(vertexshader);

	glDeleteTextures(1, &tex);
	glDeleteBuffers(2, glbo);
	glDeleteVertexArrays(1, &vao);

	shutdowndraw();
	exit(0);
}
