#include "../gltut.h"
#include "../stb_image.h"
#define MMX_IMPLEMENTATION
#define MMX_USE_DEGREES
#include "vec.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <math.h>
#include <stdint.h>
 
typedef struct Vertex Vertex;
typedef struct Mat4 Mat4;
typedef struct Vec3 Vec3;

struct Vertex {
	struct {
		GLfloat x, y;
	} position;
	struct {
		GLfloat s, t;
	} texpos;
};

struct Mat4 {
	GLfloat m[16];
};

struct Vec3 {
	GLfloat x, y, z;
};

Vertex vertices[] = {
	{{-0.5,  0.5}, {0.0, 0.0}},     // top left
	{{ 0.5,  0.5}, {1.0, 0.0}},     // top right
	{{ 0.5, -0.5}, {1.0, 1.0}},     // bottom right
	{{-0.5, -0.5}, {0.0, 1.0}},     // bottom left
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
	glerrchk("%s: bind failed", __func__);

	image = stbi_load(file, &x, &y, NULL, STBI_rgb);
	if(image == NULL) {
		fprintf(stderr, "Could not load glenda\n");
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glerrchk("%s", __func__);

	stbi_image_free(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glerrchk("%s", __func__);
}

GLuint
bindtriangle(void)
{
        GLuint vbo;

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glerrchk("%s", __func__);
        return vbo;
}

GLuint
bindindices(void)
{
        GLuint ebo;

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
		glerrchk("%s: $d", __func__, ebo);
        return ebo;
}

GLuint
linkshaders(GLuint vshadr, GLuint fshadr)
{
	GLuint shaderprog;

	shaderprog = glCreateProgram();
	glAttachShader(shaderprog, vshadr);
	glAttachShader(shaderprog, fshadr);
	glBindFragDataLocation(shaderprog, 0, "outColor");
	glLinkProgram(shaderprog);
	glUseProgram(shaderprog);
	glerrchk("%s", __func__);

	return shaderprog;
}

void
setview(GLuint shaderprog)
{
	GLint view;
	Mat4 viewmatrix;
	Vec3 eye, center, up;

	eye = (Vec3){1.2f, 1.2f, 1.2f};
	center = (Vec3){0.0f, 0.0f, 0.0f};
	up = (Vec3){0.0f, 0.0f, 1.0f};
	xm4_lookat(xm(viewmatrix), xv(eye), xv(center), xv(up));
	view = getuni(shaderprog, "view");
	glUniformMatrix4fv(view, 1, GL_FALSE, xm(viewmatrix));
}

void
setproj(GLuint shaderprog)
{
	GLint proj;
	Mat4 projmatrix;

	xm4_persp(xm(projmatrix), 45.0f, 1.0f, 1.0f, 10.0f);
	proj = getuni(shaderprog, "proj");
	glUniformMatrix4fv(proj, 1, GL_FALSE, xm(projmatrix));
}

void
setvertattrib(GLuint shaderprog, char *attrstr, int nelem, size_t offset)
{
	GLint attr;

	attr = glGetAttribLocation(shaderprog, attrstr);
	if(attr == -1) {
		fprintf(stderr, "%s: could not get attribute %s\n", __func__, attrstr);
		exit(1);
	}
	glEnableVertexAttribArray(attr);
	glVertexAttribPointer(attr, nelem, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offset);
}

int
uiloop(GLint model, GLint scale)
{
	SDL_Event e;
	SDL_Keycode keysym;
	Mat4 rotmatrix, scalematrix;
	float rot, scalef;

	rot = 0;
	for(;;) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		xm4_rotate_z(xm(rotmatrix), rot);
		glUniformMatrix4fv(model, 1, GL_TRUE, xm(rotmatrix));

		scalef = sinf(MMX_DEG2RAD(rot))+1.3;
		xm4_scalev(xm(scalematrix), scalef, scalef, scalef);
		glUniformMatrix4fv(scale, 1, GL_TRUE, xm(scalematrix));

		rot += 3;
		if(rot >= 360)
			rot = 0;

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
	}
}

int
main(void)
{
	GLuint shaderprog, vshadr, fshadr, vao, vbo, ebo, textures[2];
	GLint model, scale;

	initdraw("Transform with MMX");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glerrchk("%s: bind of vao failed", __func__);

	vshadr = compileshader("tilted.vert", GL_VERTEX_SHADER);
	fshadr = compileshader("colors.frag", GL_FRAGMENT_SHADER);
	shaderprog = linkshaders(vshadr, fshadr);

	vbo = bindtriangle();
	ebo = bindindices();
	setvertattrib(shaderprog, "position", 2, offsetof(Vertex, position));
	setvertattrib(shaderprog, "texpos", 2, offsetof(Vertex, texpos));

	glGenTextures(2, textures);
	bindtexture("../glenda.gif", textures[0], 0);
	setuni1i(shaderprog, "glendatex", 0);
	bindtexture("../sample.png", textures[1], 1);
	setuni1i(shaderprog, "kittytex", 1);

	setview(shaderprog);
	setproj(shaderprog);
	model = getuni(shaderprog, "model");
	scale = getuni(shaderprog, "scale");
	uiloop(model, scale);

	glDeleteProgram(shaderprog);
	glDeleteShader(fshadr);
	glDeleteShader(vshadr);
	glDeleteTextures(2, textures);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	shutdowndraw();

	exit(0);
}
