#include "graphics.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

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

int drawRect(GLuint img, int x, int y, int w, int h) {
    auto x0 = (GLdouble) x;
    auto y0 = (GLdouble) y;
    auto x1 = (GLdouble) (x + w);
    auto y1 = (GLdouble) (y + h);

    glBindTexture(GL_TEXTURE_2D, img);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex3d(x0, y0, 0.0);
    glTexCoord2d(1.0, 0.0);
    glVertex3d(x1, y0, 0.0);
    glTexCoord2d(1.0, 1.0);
    glVertex3d(x1, y1, 0.0);
    glTexCoord2d(0.0, 1.0);
    glVertex3d(x0, y1, 0.0);
    glEnd();

    return 0;
}


int drawPartialVerticalRect(GLuint img, int x, int y, int w, int h, double fraction) {
    auto x0 = (GLdouble) x;
    auto y0 = (GLdouble) y + (1.0-fraction)*h;
    auto x1 = (GLdouble) (x + w);
    auto y1 = (GLdouble) (y + h);

    glBindTexture(GL_TEXTURE_2D, img);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 1.0-fraction);
    glVertex3d(x0, y0, 0.0);
    glTexCoord2d(1.0, 1.0-fraction);
    glVertex3d(x1, y0, 0.0);
    glTexCoord2d(1.0, 1.0);
    glVertex3d(x1, y1, 0.0);
    glTexCoord2d(0.0, 1.0);
    glVertex3d(x0, y1, 0.0);
    glEnd();

    return 0;
}

void drawCharacterScaled(GLuint font, int x, int y, char c, double scale){
    auto x0 = (GLdouble) x;
    auto y0 = (GLdouble) y;
    auto x1 = (GLdouble) (x + 16*scale);
    auto y1 = (GLdouble) (y + 16*scale);

    glBindTexture(GL_TEXTURE_2D, font);

    auto cX = c%16;
    auto cY = c/16;

    auto xStart = cX/16.0;
    auto xEnd = (cX+1)/16.0;
    auto yStart = cY/16.0;
    auto yEnd = (cY+1)/16.0;

    glBegin(GL_QUADS);
    glTexCoord2d(xStart, yStart);
    glVertex3d(x0, y0, 0.0);
    glTexCoord2d(xEnd, yStart);
    glVertex3d(x1, y0, 0.0);
    glTexCoord2d(xEnd, yEnd);
    glVertex3d(x1, y1, 0.0);
    glTexCoord2d(xStart, yEnd);
    glVertex3d(x0, y1, 0.0);
    glEnd();
}

void drawText(GLuint font, string data,int x, int y){
    for(int i=0;i<data.size();i++){
        drawCharacterScaled(font, x+i*15, y, data[i], 1);
    }
}
void drawTextScaled(GLuint font, string data,int x, int y, double scale){
    for(int i=0;i<data.size();i++){
        drawCharacterScaled(font, x+i*15*scale, y, data[i], scale);
    }
}