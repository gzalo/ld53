#include "graphics.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
using namespace std;

const char widthPerChar[256] ={1, 7, 7, 7, 7, 7, 7, 7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 7, 7, 7, 7, 1, 4, 4, 7, 7, 2, 7, 1, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 2, 4, 7, 4, 6, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 5, 3, 7, 3, 7, 7, 3, 5, 5, 5, 5, 5, 5, 5, 5, 1, 5, 4, 2, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 4, 5, 4, 1, 4, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 1, 5, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 7, 5, 1, 1, 1, 5, 1, 5, 1, 1, 1, 1, 1, 7, 5, 1, 5, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 2, 1, 1, 1, 5, 1, 5, 1, 1, 1, 1, 1, 6, 5, 1, 5, 1, 1, 1,};

SDL_Surface * loadImage(const string &filename) {
    int req_format = STBI_rgb_alpha;
    int width = 0;
    int height = 0;
    int orig_format;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &orig_format, req_format);
    return SDL_CreateRGBSurfaceWithFormatFrom((void *) data, width, height, 32,4 * width, SDL_PIXELFORMAT_RGBA32);
}

GLuint createTextureFromSurface(const SDL_Surface *surface){
    GLuint imgId;
    glGenTextures(1, &imgId);
    glBindTexture(GL_TEXTURE_2D, imgId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 surface->pixels);
    return imgId;
}

GLuint createFontTextureFromSurface(const SDL_Surface *surface){
    GLuint imgId;
    glGenTextures(1, &imgId);
    glBindTexture(GL_TEXTURE_2D, imgId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 surface->pixels);
    return imgId;
}

int drawRect(GLuint img, double x, double y, double w, double h) {
    auto x0 = x;
    auto y0 = y;
    auto x1 = x + w;
    auto y1 = y + h;

    glBindTexture(GL_TEXTURE_2D, img);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(x0, y0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(x1, y0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(x1, y1, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(x0, y1, 0.0);
    glEnd();

    return 0;
}

int drawRectPot(GLuint img, double x, double y, double w, double h, double fractionW, double fractionH) {
    auto x0 = x;
    auto y0 = y;
    auto x1 = x + w;
    auto y1 = y + h;

    glBindTexture(GL_TEXTURE_2D, img);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(x0, y0, 0.0);
    glTexCoord2f(fractionW, 0.0);
    glVertex3f(x1, y0, 0.0);
    glTexCoord2f(fractionW, fractionH);
    glVertex3f(x1, y1, 0.0);
    glTexCoord2f(0.0, fractionH);
    glVertex3f(x0, y1, 0.0);
    glEnd();

    return 0;
}


int drawPartialVerticalRect(GLuint img, double x, double y, double w, double h, double fraction) {
    auto x0 = x;
    auto y0 = y + (1.0-fraction)*h;
    auto x1 = x + w;
    auto y1 = y + h;

    glBindTexture(GL_TEXTURE_2D, img);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0-fraction);
    glVertex3f(x0, y0, 0.0);
    glTexCoord2f(1.0, 1.0-fraction);
    glVertex3f(x1, y0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(x1, y1, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(x0, y1, 0.0);
    glEnd();

    return 0;
}


int drawPartialHorizontalRect(GLuint img, double x, double y, double w, double h, double fraction) {
    auto x0 = x;
    auto y0 = y;
    auto x1 = x + fraction*w;
    auto y1 = y + h;

    glBindTexture(GL_TEXTURE_2D, img);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(x0, y0, 0.0);
    glTexCoord2f(fraction, 0.0);
    glVertex3f(x1, y0, 0.0);
    glTexCoord2f(fraction, 1.0);
    glVertex3f(x1, y1, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(x0, y1, 0.0);
    glEnd();

    return 0;
}

void drawCharacterScaled(GLuint font, double x, double y, char c, double scale){
    auto x0 = x;
    auto y0 = y;
    auto x1 = x + 16.0*scale;
    auto y1 = y + 16.0*scale;

    glBindTexture(GL_TEXTURE_2D, font);

    auto cX = c%16;
    auto cY = c/16;

    auto xStart = cX/16.0;
    auto xEnd = (cX+1)/16.0;
    auto yStart = cY/16.0;
    auto yEnd = (cY+1)/16.0;

    glBegin(GL_QUADS);
    glTexCoord2f(xStart, yStart);
    glVertex3f(x0, y0, 0.0);
    glTexCoord2f(xEnd, yStart);
    glVertex3f(x1, y0, 0.0);
    glTexCoord2f(xEnd, yEnd);
    glVertex3f(x1, y1, 0.0);
    glTexCoord2f(xStart, yEnd);
    glVertex3f(x0, y1, 0.0);
    glEnd();
}

void drawText(GLuint font, string data, double x, double y){
    drawTextScaled(font, data, x, y, 1);
}
void drawNumericText(GLuint font, string data, double x, double y){
    for(int i=0;i<data.size();i++){
        drawCharacterScaled(font, x, y, data[i], 0.5);
        x += widthPerChar[data[i]]+1;
    }
}
void drawTextScaled(GLuint font, string data, double x, double y, double scale){
    for(int i=0;i<data.size();i++){
        drawCharacterScaled(font, x, y, data[i], scale);
        x += (widthPerChar[data[i]]+1)*2*scale;
    }
}