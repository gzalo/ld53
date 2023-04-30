#include <GL/gl.h>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>
#include <iomanip>
#include "Game.h"
#include "graphics.h"
#include "collisions.h"
#include <iostream>
#include <cmath>
#include <sstream>
using namespace std;
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

double random(double min, double max){
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}
int randomInt(int min, int max){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}
double random01(){
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> unif(0, 1);
    return unif(gen);
}

void mainLoopCallback(void *arg) {
    auto game = static_cast<Game*>(arg);

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
    }
#endif
    return 0;
}

Game::Game(const int width, const int height): screenWidth(width), screenHeight(height){
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Energy Delivery",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_OPENGL |SDL_WINDOW_RESIZABLE);

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
        exit(1);
    }
    Mix_Volume(-1, MIX_MAX_VOLUME*0.9);
    Mix_VolumeMusic(MIX_MAX_VOLUME*0.9);

    SDL_GL_CreateContext(window);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4d(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.9f, 0.9f, 0.9f, 0.1f);
    glEnable(GL_TEXTURE_2D);

    auto backgroundSurface = loadImage("res/background.png");
    backgroundTexture = createTextureFromSurface(backgroundSurface);

    auto pausedSurface = loadImage("res/paused.png");
    pausedTexture = createTextureFromSurface(pausedSurface);

    auto sink0Surface = loadImage("res/sink0.png");
    sinkTextures.push_back(createFontTextureFromSurface(sink0Surface));
    auto sink1Surface = loadImage("res/sink1.png");
    sinkTextures.push_back(createFontTextureFromSurface(sink1Surface));
    auto sink2Surface = loadImage("res/sink2.png");
    sinkTextures.push_back(createFontTextureFromSurface(sink2Surface));
    auto sink3Surface = loadImage("res/sink3.png");
    sinkTextures.push_back(createFontTextureFromSurface(sink3Surface));

    auto source0Surface = loadImage("res/source0.png");
    sourceTextures.push_back(createFontTextureFromSurface(source0Surface));
    auto source1Surface = loadImage("res/source1.png");
    sourceTextures.push_back(createFontTextureFromSurface(source1Surface));
    auto source2Surface = loadImage("res/source2.png");
    sourceTextures.push_back(createFontTextureFromSurface(source2Surface));

    auto fontSurface = loadImage("res/font.png");
    fontTexture = createFontTextureFromSurface(fontSurface);

    auto intro0Surface = loadImage("res/intro0.png");
    intro0Texture = createFontTextureFromSurface(intro0Surface);
    auto intro1Surface = loadImage("res/intro1.png");
    intro1Texture = createFontTextureFromSurface(intro1Surface);
    auto intro2Surface = loadImage("res/intro2.png");
    intro2Texture = createFontTextureFromSurface(intro2Surface);
    auto intro3Surface = loadImage("res/intro3.png");
    intro3Texture = createFontTextureFromSurface(intro3Surface);
    auto gameOverSurface = loadImage("res/gameover.png");
    gameOverTexture = createFontTextureFromSurface(gameOverSurface);

    auto health0Surface = loadImage("res/health0.png");
    health0Texture = createFontTextureFromSurface(health0Surface);
    auto health1Surface = loadImage("res/health1.png");
    health1Texture = createFontTextureFromSurface(health1Surface);

    auto gauge0Surface = loadImage("res/gauge0.png");
    gauge0Texture = createFontTextureFromSurface(gauge0Surface);
    auto gauge1Surface = loadImage("res/gauge1.png");
    gauge1Texture = createFontTextureFromSurface(gauge1Surface);
    auto gauge2Surface = loadImage("res/gauge2.png");
    gauge2Texture = createFontTextureFromSurface(gauge2Surface);

    auto btnAddSurface = loadImage("res/btnadd.png");
    btnAddTexture = createFontTextureFromSurface(btnAddSurface);
    auto btnRemSurface = loadImage("res/btnrem.png");
    btnRemTexture = createFontTextureFromSurface(btnRemSurface);
    auto btnPauseSurface = loadImage("res/btnpause.png");
    btnPauseTexture = createFontTextureFromSurface(btnPauseSurface);

    createWireSfx = Mix_LoadWAV("res/sfx/create_wire.wav");
    deleteWireSfx = Mix_LoadWAV("res/sfx/delete_wire.wav");
    clickSfx = Mix_LoadWAV("res/sfx/click.wav");
    gameOverSfx = Mix_LoadWAV("res/sfx/game_over.wav");
    levelUpSfx = Mix_LoadWAV("res/sfx/level_up.wav");
    warningSfx = Mix_LoadWAV("res/sfx/warning.wav");
    menuMusic = Mix_LoadMUS("res/menu_music.wav");
    music = Mix_LoadMUS("res/music.wav");

    levelInit();

    Mix_PlayMusic(menuMusic, -1 );
}

Game::~Game(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::handleKeyUp(const SDL_Event &event) {
    switch (event.key.keysym.sym) {
        case SDLK_SPACE:
            handleSpaceOrLeftButton();
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

void Game::handleKeyDown(const SDL_Event &event) {
    switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            if(state == GameState::PLAYING){
                pauseGame();
            } else if(state == GameState::PAUSED){
                resumeGame();
            }
            break;
        case SDLK_w:
            upKey = true;
            break;
        case SDLK_s:
            downKey = true;
            break;
        case SDLK_m:
            mute = !mute;
            Mix_Volume(-1, mute ? 0 : MIX_MAX_VOLUME*0.9);
            Mix_VolumeMusic(mute ? 0 : MIX_MAX_VOLUME*0.9);
            break;
        case SDLK_g:
            state = GameState::GAMEOVER;
            Mix_PlayChannel( -1, gameOverSfx, 0 );
            Mix_HaltMusic();
            break;
        default:
            break;
    }
}

void Game::resumeGame() {
    Mix_PlayChannel(-1, clickSfx, 0 );
    Mix_ResumeMusic();
    state = GameState::PLAYING;
}

void Game::pauseGame() {
    Mix_PlayChannel(-1, clickSfx, 0 );
    Mix_PauseMusic();
    state = GameState::PAUSED;
}


int Game::mainLoop() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Event handling

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return 0;
        if (event.window.event == SDL_WINDOWEVENT_RESIZED){
            screenWidth = event.window.data1;
            screenHeight = event.window.data2;

            glViewport(0,0,screenWidth, screenHeight);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, screenWidth, screenHeight, 0, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        if (event.type == SDL_KEYDOWN){
            handleKeyDown(event);
        }
        if (event.type == SDL_KEYUP) {
            handleKeyUp(event);
        }
        if(state == GameState::PLAYING) {
            handlePlayingMouse(event);
        } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
            handleSpaceOrLeftButton();
        }
        if(event.type == SDL_MOUSEMOTION) {
            mouseX = event.motion.x;
            mouseY = event.motion.y;

            handleMouseMotion();
        }
    }

    // Camera movement
    /*if(state == GameState::PLAYING) {
        mCamYOffset -= mCamY;
        if (upKey) mCamYOffset += 10;
        if (downKey) mCamYOffset -= 10;
    }
    if(mCamYOffset > 0 ) mCamYOffset = 0;
    if(mCamYOffset < -maxScroll + screenHeight) mCamYOffset = -maxScroll + screenHeight;*/

    mouseXWorld = mouseX;
    mouseYWorld = mouseY - mCamYOffset;

    // Game logic

    timeFrames++;
    if(timeFrames%30 == 0 && state == GameState::PLAYING){
        updateSinksAndSources();
        updateCables();
        calculateEnergyDelivery();
        score++;
        if(score % 10 == 0){
            levelUp();
        }
        if(health <= 0){
            state = GameState::GAMEOVER;
            Mix_PlayChannel( -1, gameOverSfx, 0 );
            Mix_HaltMusic();
        }
    }

    // Render
    drawRectPot(backgroundTexture, 0, 0, screenWidth, screenHeight, 800.0/1024.0, 600.0/1024.0);

    glTranslated(0, mCamYOffset, 0);
    if(dragStart != nullptr){
        if(state != GameState::PLAYING) {
            dragStart = nullptr;
        } else {
            renderPendingCable();
        }
    }
    if(state == GameState::PLAYING || state == GameState::PAUSED || state == GameState::GAMEOVER) {
        renderCables();
        renderSinks();
        renderSources();
    }

    glLoadIdentity();
    renderUi();

    SDL_GL_SwapWindow(window);
    SDL_Delay(1000 / 60);
    return 1;
}

void Game::handlePlayingMouse(const SDL_Event &event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        onMouseLeftDown();
    }
    if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button == SDL_BUTTON_LEFT)
            onMouseLeftUp();
        if (event.button.button == SDL_BUTTON_RIGHT)
            deleteClosestCable();
    }
}

void Game::renderSinks() const{
    for(auto & sink : sinks) {
        if(sink->warning){
            glColor4d(1.0, 0.0, 0.0, 1.0);
        } else {
            glColor4d(1.0, 1.0, 1.0, 1.0);
        }
        drawRect(sinkTextures[sink->type], sink->xPos, sink->yPos, elementWidth, elementHeight);
/*
        stringstream ss;
        char direction = getDirectionChar(sink->direction);
        ss << direction << setw(2) << round(sink->required);
        glColor4d(0.0, 0.0, 0.0, 1.0);
        drawNumericText(fontTexture, ss.str(), sink->xPos + elementWidth/2-4, sink->yPos+elementHeight-4);
*/
        glColor4d(1.0, 1.0, 1.0, 1.0);
        drawRect(gauge0Texture, sink->xPos, sink->yPos, 4, 64);
        drawPartialVerticalRect(gauge1Texture, sink->xPos, sink->yPos, 4, 64, sink->required / maxRequiredEver);

    }
    glColor4d(1.0, 1.0, 1.0, 1.0);
}

void Game::renderSources() const{
    for(auto & source : sources) {
        glColor4d(1.0, 1.0, 1.0, 1.0);
        drawRect(sourceTextures[source->type], source->xPos, source->yPos, elementWidth, elementHeight);

        /*stringstream ss;
        char direction = getDirectionChar(source->direction);
        ss << direction << setw(2) << round(source->generated);
        glColor4d(0.0, 0.0, 0.0, 1.0);
        drawNumericText(fontTexture, ss.str(), source->xPos + elementWidth/2-4, source->yPos+elementHeight-4);*/

        glColor4d(1.0, 1.0, 1.0, 1.0);
        drawRect(gauge0Texture, source->xPos, source->yPos, 4, 64);
        drawPartialVerticalRect(gauge2Texture, source->xPos, source->yPos, 4, 64, source->generated / maxGeneratedEver);
    }
    glColor4d(1.0, 1.0, 1.0, 1.0);
}

void Game::renderCables() const{
    glColor4d( 0, 0, 0, 255 );

    for(auto const& c : cables) {
        glLineWidth((float)c.size);

        glDisable(GL_TEXTURE_2D);
        setCableColor(c.health);

        auto dx = c.right->xPos - c.left->xPos;
        auto dy = c.right->yPos - c.left->yPos;
        auto len = sqrt(dx*dx+dy*dy);
        auto normalX = -dy/len;
        auto normalY = dx/len;

        auto offsetX = (c.offset-1)*5 * normalX;
        auto offsetY = (c.offset-1)*5 * normalY;

        glBegin(GL_LINES);
        glVertex3f(c.left->xPos+elementWidth/2+offsetX,
                   c.left->yPos+elementHeight/2+offsetY, 0);
        glVertex3f(c.right->xPos+elementWidth/2+offsetX,
                   c.right->yPos+elementHeight/2+offsetY, 0);
        glEnd();

    }
    glColor4d(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
}

void Game::onMouseLeftDown() {
    if(addWires) {
        dragStart = findClosestSource();
    }
}

void Game::onMouseLeftUp() {
    if(mouseX > xBtn + xBtnSeparation*2 && mouseX < xBtn + xBtnSeparation*2 + wBtn &&
       mouseY > yBtn && mouseY < yBtn + hBtn){
        pauseGame();
    }
    if(mouseX > xBtn && mouseX < xBtn + wBtn &&
       mouseY > yBtn && mouseY < yBtn + hBtn){
        addWires = true;
        dragStart = nullptr;
    }
    if(mouseX > xBtn + xBtnSeparation && mouseX < xBtn + xBtnSeparation + wBtn &&
       mouseY > yBtn && mouseY < yBtn + hBtn){
        addWires = false;
        dragStart = nullptr;
    }

    if(!addWires){
        deleteClosestCable();
    }

    if(addWires && dragStart != nullptr){
        Sink *dragEnd = findClosestSink();
        if(dragEnd == nullptr){
            dragStart = nullptr;
            return;
        }

        // -1 (not valid)
        // 0 -> -5
        // 1 -> 0
        // 2 -> 5
        int offset = calculateAndUpdateOffset(dragStart, dragEnd);
        if(offset == -1){
            dragStart = nullptr;
            return;
        }

        Cable c;
        c.left = dragStart;
        c.right = dragEnd;
        c.size = selectedWireSize;
        c.offset = offset;
        c.health = 5;
        cables.push_back(c);
        dragStart = nullptr;
        Mix_PlayChannel( -1, createWireSfx, 0 );
    }
}

Source *Game::findClosestSource() {
    for(auto & source : sources){
        if( ( mouseXWorld > source->xPos ) && ( mouseXWorld < source->xPos + elementWidth ) &&
            ( mouseYWorld > source->yPos ) && ( mouseYWorld < source->yPos + elementHeight ) ) {
            return source;
        }
    }
    return nullptr;
}

Sink *Game::findClosestSink() {
    for(auto & sink : sinks){
        if( ( mouseXWorld > sink->xPos ) && ( mouseXWorld < sink->xPos + elementWidth ) &&
            ( mouseYWorld > sink->yPos ) && ( mouseYWorld < sink->yPos + elementHeight ) ) {
            return sink;
        }
    }
    return nullptr;
}

void Game::renderPendingCable() const{
    glColor4d( 0, 0, 0, 1.0 );

    glLineWidth((float)selectedWireSize);

    glDisable(GL_TEXTURE_2D);
    glColor4d(0.0, 0.0, 0.0, 1.0);

    glBegin(GL_LINES);
    glVertex3f(dragStart->xPos+elementWidth/2,
               dragStart->yPos+elementHeight/2, 0);
    glVertex3f(mouseXWorld,
               mouseYWorld, 0);
    glEnd();

    glColor4d(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
}

void Game::deleteClosestCable() {
    int cableIndex = findClosestCable();

    if(cableIndex != -1){
        Cable c = cables[cableIndex];
        removeOffset(c.left, c.right, c.offset);
        cables.erase(cables.begin() + cableIndex);
        Mix_PlayChannel( -1, deleteWireSfx, 0 );
    }
}

int Game::findClosestCable() {
    int cableIdx = -1;
    double minDistance = INFINITY;
    for(int i=0;i<cables.size();i++){
        double distance = getCableDistance(i);
        if(distance < minDistance){
            minDistance = distance;
            cableIdx = i;
        }
    }
    if(minDistance > 5)
        return -1;
    return cableIdx;
}


double Game::getCableDistance(int i) {
    Source const *source = cables[i].left;
    Sink const *sink = cables[i].right;

    auto dx = sink->xPos - source->xPos;
    auto dy = sink->yPos - source->yPos;
    auto len = sqrt(dx*dx+dy*dy);
    auto normalX = -dy/len;
    auto normalY = dx/len;

    auto offsetX = (cables[i].offset-1)*5 * normalX;
    auto offsetY = (cables[i].offset-1)*5 * normalY;

    return distance_between_line_and_point(make_pair(source->xPos+elementWidth/2+offsetX, source->yPos+elementHeight/2+offsetY),
                                           make_pair(sink->xPos+elementWidth/2+offsetX, sink->yPos+elementHeight/2+offsetY),
                                           make_pair(mouseXWorld, mouseYWorld));
}

int Game::calculateAndUpdateOffset(Source *pSource, Sink *pSink) {
    auto key = make_pair(pSource, pSink);
    auto offsetMask = offsets[key];

    if(offsetMask == 0b000){
        offsets[key] = 0b010;
        return 1;
    }

    if(offsetMask == 0b001){
        offsets[key] = 0b011;
        return 1;
    }

    if(offsetMask == 0b010){
        offsets[key] = 0b011;
        return 0;
    }

    if(offsetMask == 0b011){
        offsets[key] = 0b111;
        return 2;
    }

    if(offsetMask == 0b100){
        offsets[key] = 0b110;
        return 1;
    }

    if(offsetMask == 0b101){
        offsets[key] = 0b111;
        return 1;
    }

    if(offsetMask == 0b110){
        offsets[key] = 0b111;
        return 0;
    }

    // 0b111 or others
    return -1;
}

void Game::updateSinksAndSources() const{
    for(auto & sink : sinks){
        sink->direction += random(-1, 1) * 0.2;
        sink->required += sink->direction;
        sink->required = clamp(sink->required, 0.0, maxRequiredEver);

        if(sink->required <= 0.1 && sink->direction < 0){
            sink->direction *= -1;
        }
    }
    for(auto & source : sources){
        source->direction += random(-1, 1) * 0.2;
        source->generated += source->direction;
        source->generated = clamp(source->generated, 0.0, maxGeneratedEver);

        if(source->generated <= 0.1 && source->direction < 0){
            source->direction *= -1;
        }
    }
}

char Game::getDirectionChar(double direction) const{
    if(direction > 0.8) return 1;
    if(direction < -0.8) return 2;
    if(direction > 0.5) return 5;
    if(direction < -0.5) return 6;
    return 0;
}

void Game::renderUi() const{
    if(state == GameState::INTRO0){
        drawRectPot(intro0Texture, 0, 0, screenWidth, screenHeight, 800.0/1024.0, 600.0/1024.0);
    } else if(state == GameState::INTRO1){
        drawRectPot(intro1Texture, 0, 0, screenWidth, screenHeight, 800.0/1024.0, 600.0/1024.0);
    } else if(state == GameState::INTRO2){
        drawRectPot(intro2Texture, 0, 0, screenWidth, screenHeight, 800.0/1024.0, 600.0/1024.0);
    }else if(state == GameState::INTRO3){
        drawRectPot(intro3Texture, 0, 0, screenWidth, screenHeight, 800.0/1024.0, 600.0/1024.0);
    } else if(state == GameState::PLAYING || state == GameState::PAUSED){
        drawRect(health0Texture, 538, 16, 256, 32);
        drawPartialHorizontalRect(health1Texture, 538, 16, 256, 32, health/100.0);

        if(addWires) {
            glColor4d(0.6, 1.0, 0.6, 1.0);
        } else {
            glColor4d(1.0, 1.0, 1.0, 1.0);
        }
        drawRect(btnAddTexture, xBtn, yBtn, wBtn, hBtn);

        if(!addWires) {
            glColor4d(0.6, 1.0, 0.6, 1.0);
        } else {
            glColor4d(1.0, 1.0, 1.0, 1.0);
        }
        drawRect(btnRemTexture, xBtn + xBtnSeparation, yBtn, wBtn, hBtn);

        glColor4d(1.0, 1.0, 1.0, 1.0);
        drawRect(btnPauseTexture, xBtn + 2*xBtnSeparation, yBtn, wBtn, hBtn);

        stringstream ssScore;
        ssScore << "Score: " << score;
        glColor4d(0.0, 0.0, 0.0, 1.0);
        drawText(fontTexture, ssScore.str(), 10, 6);

        stringstream ssGenerators;
        ssGenerators << "Generators: " << sources.size();
        glColor4d(0.0, 0.0, 0.0, 1.0);
        drawText(fontTexture, ssGenerators.str(), 10, 26);

        stringstream ssCities;
        ssCities << "Cities: " << sinks.size();
        glColor4d(0.0, 0.0, 0.0, 1.0);
        drawText(fontTexture, ssCities.str(), 10, 46);

        if(state == GameState::PAUSED){
            glColor4d(1.0, 1.0, 1.0, 1.0);
            drawRectPot(pausedTexture, 0, 0, screenWidth, screenHeight, 800.0/1024.0, 600.0/1024.0);
        }
    }

    glColor4d(1.0, 1.0, 1.0, 1.0);
    if(state == GameState::GAMEOVER){
        drawRectPot(gameOverTexture, 0, 0, screenWidth, screenHeight, 800.0/1024.0, 600.0/1024.0);

        stringstream ssScore;
        ssScore << score;
        drawTextScaled(fontTexture, ssScore.str(), 420, 333, 2);
    }

}

void Game::setCableColor(int cableHealth) const{
    switch(cableHealth){
        case 5:
            glColor4d(0.0, 0.0, 0.0, 1);
            break;
        case 4:
            glColor4d(0.0, 0.0, 0.0, 0.8);
            break;
        case 3:
            glColor4d(0.0, 0.0, 0.0, 0.6);
            break;
        case 2:
            glColor4d(0.0, 0.0, 0.0, 0.4);
            break;
        case 1:
            glColor4d(0.0, 0.0, 0.0, 0.2);
            break;
        default:
            glColor4d(0.0, 0.0, 0.0, 1.0);
            break;
    }
}

void Game::updateCables() {
    for(auto & cable : cables){
        if(random(0, 10) > 9){
            cable.health--;
        }
        if(cable.health == 0){
            removeOffset(cable.left, cable.right, cable.offset);
        }
    }

    cables.erase(remove_if(cables.begin(), cables.end(),
                           [](const Cable & c) { return c.health == 0; }),cables.end());
}

pair<int, int> Game::getRandomGridPosition(){
    if(grid.size() >= gridW * gridH){
        // No more spaces
        return make_pair(-1, -1);
    }

    while(true) {
        int x = randomInt(0, gridW - 1);
        int y = randomInt(0, gridH - 1);

        auto pair = make_pair(x, y);
        if (!grid[pair]) {
            grid[pair] = true;
            return pair;
        }
    }
}

void Game::levelInit() {
    sinks.clear();
    sources.clear();
    cables.clear();
    offsets.clear();
    grid.clear();

    for(int i=0;i<1;i++){
        auto *s = new Sink();
        auto position = getRandomGridPosition();
        s->xPos = position.first * gridSeparationW;
        s->yPos = position.second * gridSeparationH + gridStartY;
        s->required = random(requiredMin, requiredMax);
        s->direction = 0;
        s->warning = false;
        s->type = randomInt(0, (int)sinkTextures.size()-1);
        sinks.push_back(s);
    }

    for(int i=0;i<2;i++){
        auto *s = new Source();
        auto position = getRandomGridPosition();
        s->xPos = position.first * gridSeparationW;
        s->yPos = position.second * gridSeparationH + gridStartY;
        s->generated = random(generatedMin, generatedMax);
        s->direction = 0;
        s->type = randomInt(0,(int)sourceTextures.size()-1);
        sources.push_back(s);
    }

    for(int i=0;i<2;i++){
        Cable c;
        c.left = sources[i];
        c.right = sinks[0];
        auto offset = calculateAndUpdateOffset(c.left, c.right);
        c.size = 3;
        c.offset = offset;
        c.health = 5;
        cables.push_back(c);
    }

    health = 100;
    score = 0;
}

void Game::calculateEnergyDelivery() {
    // Reset all sources and sinks
    for(auto & source : sources) {
        source->current = source->generated;
    }
    for(auto & sink : sinks) {
        sink->current = 0;
    }

    for(auto & source : sources){

        // find all cables connected to source
        vector<Cable*> connectedCables;
        for(auto & cable : cables){
            if(cable.left == source){
                connectedCables.push_back(&cable);
            }
        }

        // split energy in equal parts
        double split = source->current / (double)connectedCables.size();
        size_t connectedCablesRemaining = connectedCables.size();

        // try to fill sinks that aren't already full
        for(auto & connectedCable : connectedCables){
            auto missingDelta = connectedCable->right->required - connectedCable->right->current;
            connectedCablesRemaining--;
            if(missingDelta > split){
                // Can fit all split
                connectedCable->right->current += split;
                source->current -= split;
            } else {
                // Split is too much
                connectedCable->right->current += missingDelta;
                source->current -= missingDelta;
                // Excess energy causes split to change
                split = source->current / (double)connectedCablesRemaining;
            }
        }
    }

    // Check if any sink is not fully filled
    bool anySinkMissingEnergy = false;
    for(auto & sink : sinks) {
        auto required = sink->required;
        auto delta = abs(sink->current - required);
        if(delta > 1) {
            anySinkMissingEnergy = true;
            sink->warning = true;
        }else{
            sink->warning = false;
        }
    }

    if(anySinkMissingEnergy) {
        health--;

        if(health%25==0){
            Mix_PlayChannel( -1, warningSfx, 0 );
        }
    }
}

void Game::removeOffset(Source *pSource, Sink *pSink, int offsetVal) {
    auto key = make_pair(pSource, pSink);
    auto offset = offsets.find(key);

    if(offset == offsets.end()){
        return;
    } else{
        // Offset is 0, 1, 2
        // bit clear 0b001, 0b010, 0b100
        offsets[key] -= 1<<offsetVal;
    }
}

void Game::levelUp() {
    auto randomNum = random01();
    // 0..0.4 => add sink
    // 0.4...0.9 => add source
    // 0.9...1 => add both
    bool addSource = randomNum > 0.4;
    bool addSink = randomNum <= 0.4 || randomNum >= 0.9;

    if(addSource) {
        auto position = getRandomGridPosition();
        if(position.first != -1) {
            auto source = new Source();
            source->xPos = position.first * gridSeparationW;
            source->yPos = position.second * gridSeparationH + gridStartY;
            source->generated = random(generatedMin, generatedMax);
            source->direction = 0;
            source->type = randomInt(0, (int) sourceTextures.size() - 1);
            sources.push_back(source);
        }
    }

    if(addSink) {
        auto position = getRandomGridPosition();

        if(position.first != -1) {
            auto sink = new Sink();
            sink->xPos = position.first * gridSeparationW;
            sink->yPos = position.second * gridSeparationH + gridStartY;
            sink->required = random(requiredMin, requiredMax);
            sink->direction = 0;
            sink->warning = false;
            sink->type = randomInt(0, (int) sinkTextures.size() - 1);
            sinks.push_back(sink);
        }
    }

    Cable c;
    c.left = sources[sources.size()-1];
    c.right = sinks[sinks.size()-1];
    c.size = 3;
    c.offset = calculateAndUpdateOffset(c.left, c.right);
    c.health = 5;
    cables.push_back(c);

    Mix_PlayChannel( -1, levelUpSfx, 0 );
}

void Game::handleMouseMotion() {
    /*
    if(mouseY > screenHeight - scrollWindow){
        mCamY = (mouseY - (screenHeight - scrollWindow)) / scrollFactor;
    } else if(mouseY < scrollWindow) {
        mCamY = -(scrollWindow - mouseY) / scrollFactor;
    } else {
        mCamY = 0;
    }*/
}

void Game::handleSpaceOrLeftButton() {
    if(state == GameState::INTRO0) {
        state = GameState::INTRO1;
    } else if(state == GameState::INTRO1){
        state = GameState::INTRO2;
    } else if(state == GameState::INTRO2){
        state = GameState::INTRO3;
    } else if(state == GameState::INTRO3){
        state = GameState::PLAYING;
        levelInit();
        Mix_PlayMusic(music, -1 );
    } else if(state == GameState::GAMEOVER){
        state = GameState::INTRO0;
        Mix_PlayMusic(menuMusic, -1 );
    } else if(state == GameState::PAUSED) {
        resumeGame();
    }
    Mix_PlayChannel( -1, clickSfx, 0 );
}
