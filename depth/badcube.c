#include "../gltut.h"
#include "../stb_image.h"
#define MMX_IMPLEMENTATION
#define MMX_USE_DEGREES
#include "vec.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdint.h>
 
typedef struct CubeVertex CubeVertex;
typedef struct Mat4 Mat4;
typedef struct Vec3 Vec3;
typedef struct Vec2 Vec2;

typedef struct ShaderDat {
	GLuint vert, frag;
} ShaderDat;

typedef struct VertexDat {
	GLuint vbo, ebo;
} VertexDat;

typedef struct Object {
	GLuint prog, vao;
} Object;

struct Mat4 {
	GLfloat m[16];
};

struct Vec3 {
	GLfloat x, y, z;
};

struct Vec2 {
	GLfloat x, y;
};

struct CubeVertex {
	Vec3 position;
	Vec2 texpos;
};

CubeVertex cube[] = {
	// below
	{{-0.5,  0.5, 0.0}, {0.0, 0.0}},	// top left
	{{ 0.5,  0.5, 0.0}, {1.0, 0.0}},	// top right
	{{ 0.5, -0.5, 0.0}, {1.0, 1.0}},	// bottom right
	{{-0.5, -0.5, 0.0}, {0.0, 1.0}},	// bottom left

	// left side
	{{-0.5, -0.5, 1.0}, {0.0, 0.0}},	// top left
	{{-0.5,  0.5, 1.0}, {1.0, 0.0}},	// top right
	{{-0.5,  0.5, 0.0}, {1.0, 1.0}},	// bottom right
	{{-0.5, -0.5, 0.0}, {0.0, 1.0}},	// bottom left

	// top side
	{{-0.5,  0.5, 1.0}, {0.0, 0.0}},	// top left
	{{ 0.5,  0.5, 1.0}, {1.0, 0.0}},	// top right
	{{ 0.5,  0.5, 0.0}, {1.0, 1.0}},	// bottom right
	{{-0.5,  0.5, 0.0}, {0.0, 1.0}},	// bottom left

	// right side
	{{ 0.5,  0.5, 1.0}, {0.0, 0.0}},	// top left
	{{ 0.5, -0.5, 1.0}, {1.0, 0.0}},	// top right
	{{ 0.5, -0.5, 0.0}, {1.0, 1.0}},	// bottom right
	{{ 0.5,  0.5, 0.0}, {0.0, 1.0}},	// bottom left

	// bottom side
	{{ 0.5, -0.5, 1.0}, {0.0, 0.0}},	// top left
	{{-0.5, -0.5, 1.0}, {1.0, 0.0}},	// top right
	{{-0.5, -0.5, 0.0}, {1.0, 1.0}},	// bottom right
	{{ 0.5, -0.5, 0.0}, {0.0, 1.0}},	// bottom left

	// above
	{{-0.5,  0.5, 1.0}, {0.0, 0.0}},	// top left
	{{ 0.5,  0.5, 1.0}, {1.0, 0.0}},	// top right
	{{ 0.5, -0.5, 1.0}, {1.0, 1.0}},	// bottom right
	{{-0.5, -0.5, 1.0}, {0.0, 1.0}},	// bottom left
};

GLuint cubeind[] = {
	// below
	0, 1, 2,
	2, 3, 0,
	// left side
	4, 5, 6,
	6, 7, 4,
	// top side
	8, 9, 10,
	10, 11, 8,
	// right side
	12, 13, 14,
	14, 15, 12,
	// bottom side
	16, 17, 18,
	18, 19, 16,
	// above
	20, 21, 22,
	22, 23, 20,
};

Vec2 mirror[] = {
	{-1.0,  1.0},
	{ 1.0,  1.0},
	{ 1.0, -1.0},
	{-1.0, -1.0},
};

GLuint mirrorind[] = {
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
bindcube(void)
{
	GLuint vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	glerrchk("%s", __func__);
	return vbo;
}

GLuint
bindcubeind(void)
{
	GLuint ebo;

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeind), cubeind, GL_STATIC_DRAW);
	glerrchk("%s: %d", __func__, ebo);
	return ebo;
}

GLuint
linkshaders(ShaderDat *sdat)
{
	GLuint prog;

	prog = glCreateProgram();
	glAttachShader(prog, sdat->vert);
	glAttachShader(prog, sdat->frag);
	glBindFragDataLocation(prog, 0, "outColor");
	glLinkProgram(prog);
	glerrchk("%s", __func__);
	return prog;
}

void
setview(GLuint shaderprog)
{
	GLint view;
	Mat4 viewmatrix;
	Vec3 eye, center, up;

	eye = (Vec3){2.0f, 2.0f, 2.0f};
	center = (Vec3){0.0f, 0.0f, 0.0f};
	up = (Vec3){0.0f, 0.0f, 1.0f};
	xm4_lookat(xm(viewmatrix), xv(eye), xv(center), xv(up));
	view = getuni(shaderprog, "view");
	if(view == -1) {
		fprintf(stderr, "%s: could not get attribute view\n", __func__);
		exit(1);
	}
	glUniformMatrix4fv(view, 1, GL_FALSE, xm(viewmatrix));
}

void
setproj(GLuint shaderprog)
{
	GLint proj;
	Mat4 projmatrix;

	xm4_persp(xm(projmatrix), 45.0f, 1.0f, 1.0f, 10.0f);
	proj = getuni(shaderprog, "proj");
	if(proj == -1) {
		fprintf(stderr, "%s: could not get attribute proj\n", __func__);
		exit(1);
	}
	glUniformMatrix4fv(proj, 1, GL_FALSE, xm(projmatrix));
}

void
setvertattrib(GLuint shaderprog, char *attrstr, int nelem, size_t size, size_t offset)
{
	GLint attr;

	attr = glGetAttribLocation(shaderprog, attrstr);
	if(attr == -1) {
		fprintf(stderr, "%s: could not get attribute %s\n", __func__, attrstr);
		exit(1);
	}
	glEnableVertexAttribArray(attr);
	glVertexAttribPointer(attr, nelem, GL_FLOAT, GL_FALSE, size, (void*)offset);
}

int
uiloop(Object *cube, Object *mirror)
{
	SDL_Event e;
	SDL_Keycode keysym;
	GLint cubemodel, mirrormodel, flip;
	Mat4 rotmatrix, flipmatrix;
	float rot, rotinc;
	int rotate, ncube, nmirror;

	ncube = sizeof(cubeind)/sizeof(cubeind[0]);
	nmirror = sizeof(mirrorind)/sizeof(mirrorind[0]);

	flip = getuni(cube->prog, "flip");
	cubemodel = getuni(cube->prog, "model");
	mirrormodel = getuni(mirror->prog, "model");

	glUseProgram(cube->prog);
	xm4_identity(xm(flipmatrix));
	glUniformMatrix4fv(flip, 1, GL_TRUE, xm(flipmatrix));
	
	rotate = 1;
	rotinc = 3.0;
	rot = 0.0;
	for(;;) {
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		if(rotate) {
			xm4_rotate_z(xm(rotmatrix), rot);
			glUseProgram(cube->prog);
			glUniformMatrix4fv(cubemodel, 1, GL_TRUE, xm(rotmatrix));
			glUseProgram(mirror->prog);
			glUniformMatrix4fv(mirrormodel, 1, GL_TRUE, xm(rotmatrix));
			rot += rotinc;
			if(rot >= 360)
				rot = 0;
		}

		glBindVertexArray(cube->vao);
		glUseProgram(cube->prog);
		glDrawElements(GL_TRIANGLES, ncube, GL_UNSIGNED_INT, 0);
		glBindVertexArray(mirror->vao);
		glUseProgram(mirror->prog);
		glDrawElements(GL_TRIANGLES, nmirror, GL_UNSIGNED_INT, 0);

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
			case SDLK_EQUALS:
				rotinc++;
				break;
			case SDLK_MINUS:
				rotinc--;
				break;
			case SDLK_SPACE:
				rotate ^= 1;
				break;
			}
			break;
		}
	}
}

GLuint
makecubeprog(ShaderDat *sdat, GLuint *textures)
{
	GLuint prog;

	prog = linkshaders(sdat);
	glUseProgram(prog);

	setvertattrib(prog, "position", 3, sizeof(CubeVertex), offsetof(CubeVertex, position));
	setvertattrib(prog, "texpos", 2, sizeof(CubeVertex), offsetof(CubeVertex, texpos));

	setview(prog);
	setproj(prog);

	glGenTextures(2, textures);
	bindtexture("../glenda.gif", textures[0], 0);
	setuni1i(prog, "glendatex", 0);
	bindtexture("../sample.png", textures[1], 1);
	setuni1i(prog, "kittytex", 1);
	return prog;
}

int
main(void)
{
	Object cubeobj, mirrorobj;
	ShaderDat cubeshader, mirrorshader;
	VertexDat cubevert, mirrorvert;
	GLuint textures[2];

	initdraw("Depth and Stencil");
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &cubeobj.vao);
	glBindVertexArray(cubeobj.vao);
	glerrchk("%s: bind of vao failed", __func__);

	cubevert.vbo = bindcube();
	cubevert.ebo = bindcubeind();
	cubeshader.vert = compileshader("cube.vert", GL_VERTEX_SHADER);
	cubeshader.frag = compileshader("cube.frag", GL_FRAGMENT_SHADER);
	cubeobj.prog = makecubeprog(&cubeshader, textures);

	glGenVertexArrays(1, &mirrorobj.vao);
	glBindVertexArray(mirrorobj.vao);
	glerrchk("%s: bind of mirror vao failed", __func__);
	glGenBuffers(2, (GLuint*)&mirrorvert);
	glBindBuffer(GL_ARRAY_BUFFER, mirrorvert.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mirror), mirror, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mirrorvert.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mirrorind), mirrorind, GL_STATIC_DRAW);
	glerrchk("%s: bind of mirror vertex or element buffers failed", __func__);
	mirrorshader.vert = compileshader("mirror.vert", GL_VERTEX_SHADER);
	mirrorshader.frag = compileshader("mirror.frag", GL_FRAGMENT_SHADER);
	mirrorobj.prog = linkshaders(&mirrorshader);
	glUseProgram(mirrorobj.prog);
	setview(mirrorobj.prog);
	setproj(mirrorobj.prog);
	setvertattrib(mirrorobj.prog, "position", 2, sizeof(mirror[0]), 0);

	uiloop(&cubeobj, &mirrorobj);

	glDeleteProgram(cubeobj.prog);
	glDeleteShader(cubeshader.frag);
	glDeleteShader(cubeshader.vert);
	glDeleteTextures(2, textures);
	glDeleteBuffers(1, &cubevert.ebo);
	glDeleteBuffers(1, &cubevert.vbo);
	glDeleteVertexArrays(1, &cubeobj.vao);

	glDeleteProgram(mirrorobj.prog);
	glDeleteShader(mirrorshader.frag);
	glDeleteShader(mirrorshader.vert);
	glDeleteBuffers(2, (GLuint*)&mirrorvert);
	glDeleteVertexArrays(1, &mirrorobj.vao);
	

	shutdowndraw();

	exit(0);
}
