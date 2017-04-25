#ifdef __cplusplus
extern "C" {
#endif

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

GLuint compileshader(char*, GLenum);
int initdraw(char*);
void shutdowndraw(void);
void glerrchk(char*, ...);
GLint getuni(GLuint, char*);
void setuni1i(GLuint, char*, int);

extern SDL_Window *screen;
extern SDL_GLContext glcontext;

#ifdef __cplusplus
}
#endif
