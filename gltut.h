#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

GLuint compileshader(char*, GLenum);
int initdraw(void);
void shutdowndraw(void);

extern SDL_Window *screen;
extern SDL_GLContext glcontext;
