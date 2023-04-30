#include "Sink.h"
#include "Source.h"
#include "Cable.h"

#ifndef LD53_GAME_H
#define LD53_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <GL/gl.h>
#include <vector>
#include <map>

enum class GameState{
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
    const double requiredMax = 20.0;
    const double generatedMin = 15.0;
    const double generatedMax = 50.0;
    const double maxRequiredEver = 50.0;
    const double maxGeneratedEver = 50.0;
    const int maxScroll = 2048;
    const int selectedWireSize = 3;
    const int gridW = 12; // 800 / 66
    const int gridH = 8; // (600-64) / 67
    const int gridSeparationW = 66;
    const int gridSeparationH = 67;
    const int gridStartY = 64;

    const int yBtn = 0;
    const int xBtn = 290;
    const int xBtnSeparation = 68;
    const int wBtn = 64;
    const int hBtn = 64;

    SDL_Window *window;

    std::vector<GLuint> sinkTextures;
    std::vector<GLuint> sourceTextures;
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
    GLuint gauge0Texture;
    GLuint gauge1Texture;
    GLuint gauge2Texture;
    GLuint btnAddTexture;
    GLuint btnRemTexture;
    GLuint btnPauseTexture;
    Mix_Chunk *createWireSfx;
    Mix_Chunk *clickSfx;
    Mix_Chunk *deleteWireSfx;
    Mix_Chunk *gameOverSfx;
    Mix_Chunk *levelUpSfx;
    Mix_Chunk *warningSfx;
    Mix_Music *menuMusic;
    Mix_Music *music;
    bool mute = false;

    int mouseX = 0;
    int mouseY = 0;
    int mouseXWorld = 0;
    int mouseYWorld = 0;
    const int mCamY = 0;
    const int mCamYOffset = 0;
    bool upKey = false;
    bool downKey = false;
    bool addWires = true;

    Source *dragStart = nullptr;
    int timeFrames = 0;

    // Part of game core
    std::vector<Sink*> sinks;
    std::vector<Source*> sources;
    std::vector<Cable> cables;
    std::map<std::pair<Source*,Sink*>, int> offsets;
    std::map<std::pair<int, int>, bool> grid;
    int health = 0;
    GameState state = GameState::INTRO0;
    int score = 0;

    void handleKeyUp(const SDL_Event &event);
    void handleKeyDown(const SDL_Event &event);

    void renderSinks() const;

    void renderSources() const;

    void renderCables() const;

    void onMouseLeftDown();

    void onMouseLeftUp();

    Source *findClosestSource();

    Sink *findClosestSink();

    void renderPendingCable() const;

    void deleteClosestCable();

    int findClosestCable();

    double getCableDistance(int i);

    int calculateAndUpdateOffset(Source *pSource, Sink *pSink);

    void updateSinksAndSources() const;

    char getDirectionChar(double direction) const;

    void renderUi() const;

    void setCableColor(int health) const;

    void updateCables();

    void levelInit();

    void calculateEnergyDelivery();

    void removeOffset(Source *pSource, Sink *pSink, int offset);

    void levelUp();

    void handleMouseMotion();

    void handleSpaceOrLeftButton();

    void handlePlayingMouse(const SDL_Event &event);

    void pauseGame();

    void resumeGame();

    std::pair<int, int> getRandomGridPosition();
};

#endif
