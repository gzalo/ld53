//
// Created by Gzalo on 28/4/2023.
//

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
