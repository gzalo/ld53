#ifdef WIN32
#include <windows.h>
#include <fcntl.h>
#endif
#include "Game.h"
#include "graphics.h"
#include <iostream>
#include <cmath>
using namespace std;
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void mainLoopCallback(void *arg) {
    Game* game = static_cast<Game*>(arg);

    if(game->mainLoop() == 0){
        #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
        #endif
    }
}

int Game::run() {

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(mainLoopCallback, (void*) this, 0, true);
#else
    while(mainLoop()) {
        SDL_Delay(0);
    };
#endif
    return 0;
}

Game::Game(const int width, const int height){

    SDL_Init(SDL_INIT_EVERYTHING);

    auto window = SDL_CreateWindow("LD53",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   width, height,
                                   SDL_WINDOW_OPENGL);

    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2" );

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED );
    if ( !renderer ) {
        cout << "Error creating renderer: " << SDL_GetError() << endl;
        return;
    }

    auto background = loadImage("res/background.png");
    texture = SDL_CreateTextureFromSurface(renderer, background);

#ifdef WIN32
    AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long long)handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long long)handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
#endif

}
Game::~Game(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::handleKeyUp(const SDL_Event &event) {
    switch (event.key.keysym.sym) {
        case SDLK_a:
            leftKey = false;
            break;
        case SDLK_d:
            rightKey = false;
            break;
        case SDLK_w:
            upKey = false;
            break;
        case SDLK_s:
            downKey = false;
            break;
        default:
            break;
    }
}

bool Game::handleKeyDown(const SDL_Event &event) {
    switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            return true;
        case SDLK_a:
            leftKey = true;
            break;
        case SDLK_d:
            rightKey = true;
            break;
        case SDLK_w:
            upKey = true;
            break;
        case SDLK_s:
            downKey = true;
            break;
        default:
            break;
    }
    return false;
}


int Game::mainLoop() {
    Uint64 start = SDL_GetPerformanceCounter();
    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderClear( renderer );

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return 0;
        if (event.type == SDL_KEYDOWN && handleKeyDown(event) == 1) return 0;
        if (event.type == SDL_KEYUP) {
            handleKeyUp(event);
        }
    }

    const auto SPEED = 10;

    if(upKey) yPos-=SPEED;
    if(downKey) yPos+=SPEED;
    if(leftKey) xPos-=SPEED;
    if(rightKey) xPos+=SPEED;

    auto factor = 10*sin(timeFrames*60/M_PI);

    SDL_Rect dst;
    dst.x = xPos;
    dst.y = yPos;
    dst.w = 300 + factor;
    dst.h = 300 + factor;

    SDL_RenderCopyEx( renderer, texture, NULL, &dst, timeFrames, NULL, SDL_FLIP_NONE);

    SDL_RenderPresent(renderer);
    timeFrames++;

    Uint64 end = SDL_GetPerformanceCounter();
    float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();

    if(timeFrames % 10 == 0) {
        cout << "Current FPS: " << to_string(1.0f / elapsed) << endl;
    }
    return 1;
}