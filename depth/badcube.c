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

struct CubeVertex {
	struct {
		GLfloat x, y, z;
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
	glerrchk("%s", __func__);

	return shaderprog;
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
uiloop(GLuint cubeshader, GLuint floorshader)
{
	SDL_Event e;
	SDL_Keycode keysym;
	GLint model, flip;
	Mat4 rotmatrix, flipmatrix;
	float rot, rotinc;
	int rotate, ncube;

	ncube = sizeof(cubeind)/sizeof(cubeind[0]);

	flip = getuni(cubeshader, "flip");
	model = getuni(cubeshader, "model");

	xm4_identity(xm(flipmatrix));
	glUniformMatrix4fv(flip, 1, GL_TRUE, xm(flipmatrix));
	
	rotate = 1;
	rotinc = 3.0;
	rot = 0.0;
	for(;;) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		xm4_rotate_z(xm(rotmatrix), rot);
		glUniformMatrix4fv(model, 1, GL_TRUE, xm(rotmatrix));
		if(rotate) {
			rot += rotinc;
			if(rot >= 360)
				rot = 0;
		}

		glDrawElements(GL_TRIANGLES, ncube, GL_UNSIGNED_INT, 0);

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
				}
				break;
		}
	}
}

typedef struct ShaderDat {
	GLuint prog, vert, frag;
} ShaderDat;

typedef struct VertexDat {
	GLuint vbo, ebo;
} VertexDat;

typedef struct Object {
	GLuint prog, vao;
} Object;

void
makecubeprog(ShaderDat *sdat, GLuint *textures)
{
	sdat->vert = compileshader("cube.vert", GL_VERTEX_SHADER);
	sdat->frag = compileshader("cube.frag", GL_FRAGMENT_SHADER);
	sdat->prog = linkshaders(sdat->vert, sdat->frag);
	glUseProgram(sdat->prog);

	setvertattrib(sdat->prog, "position", 3, sizeof(CubeVertex), offsetof(CubeVertex, position));
	setvertattrib(sdat->prog, "texpos", 2, sizeof(CubeVertex), offsetof(CubeVertex, texpos));

	setview(sdat->prog);
	setproj(sdat->prog);

	glGenTextures(2, textures);
	bindtexture("../glenda.gif", textures[0], 0);
	setuni1i(sdat->prog, "glendatex", 0);
	bindtexture("../sample.png", textures[1], 1);
	setuni1i(sdat->prog, "kittytex", 1);
}

int
main(void)
{
	ShaderDat cubeshader, floorshader;
	VertexDat cubevert, floorvert;
	GLuint vao, textures[2];

	initdraw("Transform with MMX");
	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glerrchk("%s: bind of vao failed", __func__);

	cubevert.vbo = bindcube();
	cubevert.ebo = bindcubeind();

	makecubeprog(&cubeshader, textures);

	uiloop(cubeshader.prog, floorshader.prog);

	glDeleteProgram(cubeshader.prog);
	glDeleteShader(cubeshader.frag);
	glDeleteShader(cubeshader.vert);
	glDeleteTextures(2, textures);
	glDeleteBuffers(1, &cubevert.ebo);
	glDeleteBuffers(1, &cubevert.vbo);
	glDeleteVertexArrays(1, &vao);

	shutdowndraw();

	exit(0);
}
