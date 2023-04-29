#ifndef LD53_GRAPHICS_H
#define LD53_GRAPHICS_H

#include <SDL2/SDL.h>
#include <string>
#include <GL/gl.h>

using namespace std;

SDL_Surface * loadImage(const string &filename);
GLuint createTextureFromSurface(const SDL_Surface *surface);
GLuint createFontTextureFromSurface(const SDL_Surface *surface);
int drawRect(GLuint img, int x, int y, int w, int h);
int drawPartialVerticalRect(GLuint img, int x, int y, int w, int h, double fraction);
void drawCharacterScaled(GLuint font, int x, int y, char c, double scale);
void drawText(GLuint font, string data,int x, int y);
void drawTextScaled(GLuint font, string data,int x, int y, double scale);

#endif
