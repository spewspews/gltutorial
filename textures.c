#include "gltut.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdint.h>
 
typedef struct Vertex Vertex;
struct Vertex {
	float x, y, r, g, b, s, t;
};

Vertex vertices[] = {
	{-0.5,  0.5, 1.0, 0.0, 0.0, 0.0, 0.0},     // top left
	{ 0.5,  0.5, 0.0, 1.0, 0.0, 1.0, 0.0},     // top right
	{ 0.5, -0.5, 0.0, 0.0, 1.0, 1.0, 1.0},     // bottom right
	{-0.5, -0.5, 1.0, 1.0, 1.0, 0.0, 1.0},     // bottom left
};

GLuint elements[] = {
        0, 1, 2,
        2, 3, 0,
};

GLuint
loadglenda(void)
{
	GLuint tex;
	int x, y;
	uint8_t *image;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	image = stbi_load("glenda.gif", &x, &y, NULL, STBI_rgb);
	if(image == NULL) {
		fprintf(stderr, "Could not load glenda\n");
		abort();
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	stbi_image_free(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

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
bindindices(void)
{
        GLuint ebo;

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
        return ebo;
}

int
uiloop(void)
{
        SDL_Event e;
        SDL_Keycode keysym;

        for(;;) {
                glClearColor(0.0, 0.0, 0.0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT);

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
                SDL_Delay(100);
        }
}

int
main(void)
{
        GLuint shaderprog, vertexshader, fragmentshader, vao, vbo, ebo, tex;
        GLint position, color, texcoord;

	initdraw();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        vertexshader = compileshader("texturesshaders/flat.vert", GL_VERTEX_SHADER);
        fragmentshader = compileshader("texturesshaders/colors.frag", GL_FRAGMENT_SHADER);

        shaderprog = glCreateProgram();
        glAttachShader(shaderprog, vertexshader);
        glAttachShader(shaderprog, fragmentshader);

	glBindFragDataLocation(shaderprog, 0, "outColor");
        glLinkProgram(shaderprog);
        glUseProgram(shaderprog);

	vbo = bindtriangle();
	ebo = bindindices();
	tex = loadglenda();

        position = glGetAttribLocation(shaderprog, "position");
        glEnableVertexAttribArray(position);
        glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

        color = glGetAttribLocation(shaderprog, "color");
        glEnableVertexAttribArray(color);
        glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));

	texcoord = glGetAttribLocation(shaderprog, "texcoord");
	glEnableVertexAttribArray(texcoord);
	glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));

	uiloop();

        glDeleteProgram(shaderprog);
        glDeleteShader(fragmentshader);
        glDeleteShader(vertexshader);

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
	glDeleteTextures(1, &tex);
	glDeleteVertexArrays(1, &vao);

	shutdowndraw();
	exit(0);
}
