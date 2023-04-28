#ifndef LD53_GAME_H
#define LD53_GAME_H

#include <SDL2/SDL.h>

class Game {

public:
    int run();
    Game(int w, int h);
    ~Game();
    int mainLoop();

private:
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Window *window;

    int xPos = 0;
    int yPos = 0;

    bool leftKey = false;
    bool rightKey = false;
    bool upKey = false;
    bool downKey = false;

    int timeFrames = 0;

    void handleKeyUp(const SDL_Event &event);
    bool handleKeyDown(const SDL_Event &event);
    void runMainLoop();
};


#endif
