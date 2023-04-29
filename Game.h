#ifndef LD53_GAME_H
#define LD53_GAME_H

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <vector>
#include <map>
using namespace std;
#include "Sink.h"
#include "Source.h"
#include "Cable.h"

enum GameState{
    INTRO0,
    INTRO1,
    INTRO2,
    INTRO3,
    PLAYING,
    PAUSED,
    GAMEOVER,
};

// intro0 -> intro1 -> intro2 -> intro3 -> playing <-> paused
//    ^                                       |
//    |                                       V
//    ----------------------------------- gameover


class Game {

public:
    int run();
    Game(int w, int h);
    ~Game();
    int mainLoop();

private:

    int screenWidth;
    int screenHeight;
    const int elementWidth = 64;
    const int elementHeight = 64;
    const int scrollWindow = 50;
    const int scrollFactor = 10;

    const int verticalSeparation = 85;

    const double requiredMin = 10.0;
    const double requiredMax = 25.0;
    const double generatedMin = 10.0;
    const double generatedMax = 50.0;
    const double maxRequiredEver = 80.0;
    const double maxGeneratedEver = 100.0;
    const int maxScroll = 2048;

    SDL_Window *window;

    vector<GLuint> sinkTextures;
    vector<GLuint> sourceTextures;
    GLuint fontTexture;
    GLuint backgroundTexture;
    GLuint pausedTexture;
    GLuint intro0Texture;
    GLuint intro1Texture;
    GLuint intro2Texture;
    GLuint intro3Texture;
    GLuint gameOverTexture;
    GLuint health0Texture;
    GLuint health1Texture;

    int mouseX = 0;
    int mouseY = 0;
    int mouseXWorld = 0;
    int mouseYWorld = 0;
    int mCamY = 0;
    int mCamYOffset = 0;
    int selectedWireSize = 3;
    bool upKey = false;
    bool downKey = false;

    Source *dragStart = NULL;
    int timeFrames = 0;

    // Part of game core
    vector<Sink*> sinks;
    vector<Source*> sources;
    vector<Cable> cables;
    map<pair<Source*,Sink*>, int> offsets;
    int health = 0;
    GameState state = INTRO0;
    int score = 0;

    void handleKeyUp(const SDL_Event &event);
    void handleKeyDown(const SDL_Event &event);

    void renderSinks();

    void renderSources();

    void renderCables();

    void onMouseLeftDown();

    void onMouseLeftUp();

    Source *findClosestSource();

    Sink *findClosestSink();

    void renderPendingCable();

    void onMouseRightUp();

    int findClosestCable();

    double getCableDistance(int i);

    int calculateAndUpdateOffset(Source *pSource, Sink *pSink);

    void updateSinksAndSources();

    char getDirectionChar(double direction);

    void renderUi();

    void setCableColor(int health);

    void updateCables();

    void levelInit();

    void calculateEnergyDelivery();

    void removeOffset(Source *pSource, Sink *pSink, int offset);

    void levelUp();

    void handleMouseMotion();

    void handleSpaceKey();

};

#endif
