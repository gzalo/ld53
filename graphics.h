#ifndef LD53_GRAPHICS_H
#define LD53_GRAPHICS_H

#include <SDL2/SDL.h>
#include <string>
#include <GL/gl.h>

SDL_Surface * loadImage(const std::string &filename);
GLuint createTextureFromSurface(const SDL_Surface *surface);
GLuint createFontTextureFromSurface(const SDL_Surface *surface);
int drawRect(GLuint img, double x, double y, double w, double h);
int drawRectPot(GLuint img, double x, double y, double w, double h, double fractionW, double fractionH);
int drawPartialVerticalRect(GLuint img, double x, double y, double w, double h, double fraction);
int drawPartialHorizontalRect(GLuint img, double x, double y, double w, double h, double fraction);
void drawCharacterScaled(GLuint font, double x, double y, char c, double scale);
void drawText(GLuint font, std::string data, double x, double y);
void drawTextScaled(GLuint font, std::string data, double x, double y, double scale);
void drawNumericText(GLuint font, std::string data, double x, double y);

#endif
