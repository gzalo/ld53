#ifdef WIN32
#include <windows.h>
#include <fcntl.h>
#endif
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
#include <GL/gl.h>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>
#include <iomanip>

double random(double min, double max){
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}
double random01(){
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> unif(0, 1);
    return unif(gen);
}

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

    srand(time(NULL));

    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_OPENGL);

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

    levelInit();

    screenWidth = width;
    screenHeight = height;
}
Game::~Game(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::handleKeyUp(const SDL_Event &event) {
    switch (event.key.keysym.sym) {
        case SDLK_SPACE:
            handleSpaceKey();
            break;
        /*case SDLK_1:
            selectedWireSize = 1;
            break;
        case SDLK_2:
            selectedWireSize = 2;
            break;
        case SDLK_3:
            selectedWireSize = 3;
            break;*/
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
            if(state == PLAYING){
                state = PAUSED;
            } else if(state == PAUSED){
                state = PLAYING;
            }
            break;
        case SDLK_w:
            upKey = true;
            break;
        case SDLK_s:
            downKey = true;
            break;
        case SDLK_g:
            state = GAMEOVER;
            break;
        default:
            break;
    }
}


int Game::mainLoop() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Event handling

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return 0;
        if (event.type == SDL_KEYDOWN){
            handleKeyDown(event);
        }
        if (event.type == SDL_KEYUP) {
            handleKeyUp(event);
        }
        if(state == PLAYING) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT)
                    onMouseLeftDown();
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT)
                    onMouseLeftUp();
                if (event.button.button == SDL_BUTTON_RIGHT)
                    onMouseRightUp();
            }
        }
        if(event.type == SDL_MOUSEMOTION) {
            mouseX = event.motion.x;
            mouseY = event.motion.y;

            handleMouseMotion();
        }
    }

    // Camera movement
    if(state == PLAYING) {
        mCamYOffset -= mCamY;
        if (upKey) mCamYOffset += 10;
        if (downKey) mCamYOffset -= 10;
    }
    if(mCamYOffset > 0 ) mCamYOffset = 0;
    if(mCamYOffset < -maxScroll + screenHeight) mCamYOffset = -maxScroll + screenHeight;

    mouseXWorld = mouseX;
    mouseYWorld = mouseY - mCamYOffset;

    // Game logic

    timeFrames++;
    if(timeFrames%30 == 0 && state == PLAYING){
        updateSinksAndSources();
        updateCables();
        calculateEnergyDelivery();
        score++;
        if(score % 10 == 0){
            levelUp();
        }
        if(health <= 0){
            state = GAMEOVER;
        }
    }

    // Render
    drawRect(backgroundTexture, 0, 0, screenWidth, screenHeight);

    glTranslated(0, mCamYOffset, 0);
    if(dragStart != NULL){
        if(state != PLAYING) {
            dragStart = NULL;
        } else {
            renderPendingCable();
        }
    }
    if(state == PLAYING || state == PAUSED || state == GAMEOVER) {
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

void Game::renderSinks() {
    for(int i=0;i<sinks.size();i++) {
        if(sinks[i]->warning){
            glColor4d(1.0, 0.0, 0.0, 1.0);
        } else {
            glColor4d(1.0, 1.0, 1.0, 1.0);
        }
        glColor4d(1.0, 1.0, 1.0, 1.0);
        drawRect(sinkTextures[sinks[i]->type], sinks[i]->xPos, sinks[i]->yPos, elementWidth, elementHeight);
        stringstream ss;
        char direction = getDirectionChar(sinks[i]->direction);
        ss << direction << setw(2) << round(sinks[i]->required);
        glColor4d(0.0, 0.0, 0.0, 1.0);
        drawText(fontTexture, ss.str(), sinks[i]->xPos + elementWidth + 18, sinks[i]->yPos+24);

    }
    glColor4d(1.0, 1.0, 1.0, 1.0);
}

void Game::renderSources() {
    for(int i=0;i<sources.size();i++) {
        glColor4d(1.0, 1.0, 1.0, 1.0);
        drawRect(sourceTextures[sources[i]->type], sources[i]->xPos, sources[i]->yPos, elementWidth, elementHeight);
        stringstream ss;
        char direction = getDirectionChar(sources[i]->direction);
        ss << direction << setw(2) << round(sources[i]->generated);
        glColor4d(0.0, 0.0, 0.0, 1.0);
        drawText(fontTexture, ss.str(), sources[i]->xPos - 46, sources[i]->yPos+30);
    }
    glColor4d(1.0, 1.0, 1.0, 1.0);
}

void Game::renderCables() {
    glColor4d( 0, 0, 0, 255 );

    for(int i=0;i<cables.size();i++) {
        Cable c = cables[i];

        glLineWidth(c.size);

        glDisable(GL_TEXTURE_2D);
        setCableColor(c.health);

        glBegin(GL_LINES);
        glVertex3d(c.left->xPos+elementWidth/2,
                   c.left->yPos+elementHeight/2+(c.offset-1)*5, 0);
        glVertex3d(c.right->xPos+elementWidth/2,
                   c.right->yPos+elementHeight/2+(c.offset-1)*5, 0);
        glEnd();

    }
    glColor4d(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
}

void Game::onMouseLeftDown() {
    dragStart = findClosestSource();
}

void Game::onMouseLeftUp() {
    if(dragStart != NULL){
        Sink *dragEnd = findClosestSink();
        if(dragEnd == NULL){
            dragStart = NULL;
            return;
        }

        // -1 (not valid)
        // 0 -> -5
        // 1 -> 0
        // 2 -> 5
        int offset = calculateAndUpdateOffset(dragStart, dragEnd);
        if(offset == -1){
            dragStart = NULL;
            return;
        }

        Cable c;
        c.left = dragStart;
        c.right = dragEnd;
        c.size = selectedWireSize;
        c.offset = offset;
        c.health = 5;
        cables.push_back(c);
        dragStart = NULL;
    }
}

Source *Game::findClosestSource() {
    for(int i=0;i<sources.size();i++){
        if( ( mouseXWorld > sources[i]->xPos ) && ( mouseXWorld < sources[i]->xPos + elementWidth ) &&
            ( mouseYWorld > sources[i]->yPos ) && ( mouseYWorld < sources[i]->yPos + elementHeight ) ) {
            return sources[i];
        }
    }
    return NULL;
}

Sink *Game::findClosestSink() {
    for(int i=0;i<sinks.size();i++){
        if( ( mouseXWorld > sinks[i]->xPos ) && ( mouseXWorld < sinks[i]->xPos + elementWidth ) &&
            ( mouseYWorld > sinks[i]->yPos ) && ( mouseYWorld < sinks[i]->yPos + elementHeight ) ) {
            return sinks[i];
        }
    }
    return NULL;
}

void Game::renderPendingCable() {
    glColor4d( 0, 0, 0, 1.0 );

    glLineWidth(selectedWireSize);

    glDisable(GL_TEXTURE_2D);
    glColor4d(0.0, 0.0, 0.0, 1.0);

    glBegin(GL_LINES);
    glVertex3d(dragStart->xPos+elementWidth/2,
               dragStart->yPos+elementHeight/2, 0);
    glVertex3d(mouseXWorld,
               mouseYWorld, 0);
    glEnd();

    glColor4d(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
}

void Game::onMouseRightUp() {
    int cableIndex = findClosestCable();

    if(cableIndex != -1){
        Cable c = cables[cableIndex];
        removeOffset(c.left, c.right, c.offset);
        cables.erase(cables.begin() + cableIndex);
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
    Source *source = cables[i].left;
    Sink *sink = cables[i].right;
    return distance_between_line_and_point(make_pair(source->xPos+elementWidth/2, source->yPos+elementHeight/2+(cables[i].offset-1)*5),
                                           make_pair(sink->xPos+elementWidth/2, sink->yPos+elementHeight/2+(cables[i].offset-1)*5),
                                           make_pair(mouseXWorld, mouseYWorld));
}

int Game::calculateAndUpdateOffset(Source *pSource, Sink *pSink) {
    auto key = make_pair(pSource, pSink);
    auto offset = offsets.find(key);

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

    if(offsetMask == 0b111){
        return -1;
    }
    return -1;
}

void Game::updateSinksAndSources() {
    for(auto & sink : sinks){
        sink->direction += random(-1, 1) * 0.2;
        sink->required += sink->direction;
        sink->required = clamp(sink->required, 0.0, maxRequiredEver);
    }
    for(auto & source : sources){
        source->direction += random(-1, 1) * 0.2;
        source->generated += source->direction;
        source->generated = clamp(source->generated, 0.0, maxGeneratedEver);
    }
}

char Game::getDirectionChar(double direction) {
    if(direction > 0.8) return 1;
    if(direction < -0.8) return 2;
    if(direction > 0.5) return 5;
    if(direction < -0.5) return 6;
    return 0;
}

void Game::renderUi() {
    if(state == INTRO0){
        drawRect(intro0Texture, 0, 0, screenWidth, screenHeight);
    } else if(state == INTRO1){
        drawRect(intro1Texture, 0, 0, screenWidth, screenHeight);
    } else if(state == INTRO2){
        drawRect(intro2Texture, 0, 0, screenWidth, screenHeight);
    }else if(state == INTRO3){
        drawRect(intro3Texture, 0, 0, screenWidth, screenHeight);
    } else if(state == PLAYING || state == PAUSED){
        drawRect(health0Texture, 750, 10, 32, 256);
        drawPartialVerticalRect(health1Texture, 750, 10, 32, 256, health/100.0);

        stringstream ssScore;
        ssScore << "Score: " << score;
        glColor4d(0.0, 0.0, 0.0, 1.0);
        drawText(fontTexture, ssScore.str(), 10, 10);

        if(state == PAUSED){
            drawRect(pausedTexture, 0, 0, screenWidth, screenHeight);
        }
    }

    glColor4d(1.0, 1.0, 1.0, 1.0);
    if(state == GAMEOVER){
        drawRect(gameOverTexture, 0, 0, screenWidth, screenHeight);

        stringstream ssScore;
        ssScore << score;
        drawTextScaled(fontTexture, ssScore.str(), 420, 333, 2);
    }

}

void Game::setCableColor(int health) {
    switch(health){
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
    for(int i=0;i<cables.size();i++){
        if(random(0, 10) > 9){
            cables[i].health--;
        }
        if(cables[i].health == 0){
            removeOffset(cables[i].left, cables[i].right, cables[i].offset);
        }
    }

    cables.erase(remove_if(cables.begin(), cables.end(),
                           [](const Cable & c) { return c.health == 0; }),cables.end());
}

void Game::levelInit() {
    sinks.clear();
    sources.clear();
    cables.clear();
    offsets.clear();

    for(int i=0;i<3;i++){
        Sink *s = new Sink();
        s->xPos = 500;
        s->yPos = 100 + i * verticalSeparation;
        s->required = random(requiredMin, requiredMax);
        s->direction = 0;
        s->warning = false;
        s->type = rand()%sinkTextures.size();
        sinks.push_back(s);
    }

    for(int i=0;i<2;i++){
        Source *s = new Source();
        s->xPos = 100;
        s->yPos = 100 + i * verticalSeparation;
        s->generated = random(generatedMin, generatedMax);
        s->direction = 0;
        s->type = rand()%sourceTextures.size();
        sources.push_back(s);
    }

    for(int i=0;i<5;i++){
        Cable c;
        c.left = sources[i%2];
        c.right = sinks[i%3];
        auto offset = calculateAndUpdateOffset(c.left, c.right);
        c.size = 3;
        c.offset = offset;
        c.health = 5;
        cables.push_back(c);
    }

    health = 100;
    score = 0;
    mCamYOffset = 0;
}

void Game::calculateEnergyDelivery() {
    // Reset all sources and sinks
    for(int i=0;i<sources.size();i++) {
        sources[i]->current = sources[i]->generated;
    }
    for(int i=0;i<sinks.size();i++) {
        sinks[i]->current = 0;
    }

    for(int i=0;i<sources.size();i++){

        // find all cables connected to source
        vector<Cable*> connectedCables;
        for(int j=0;j<cables.size();j++){
            if(cables[j].left == sources[i]){
                connectedCables.push_back(&cables[j]);
            }
        }

        // split energy in equal parts
        double split = sources[i]->current / (double)connectedCables.size();
        int connectedCablesRemaining = connectedCables.size();

        // try to fill sinks that aren't already full
        for(int j=0;j<connectedCables.size();j++){
            auto missingDelta = connectedCables[j]->right->required - connectedCables[j]->right->current;
            connectedCablesRemaining--;
            if(missingDelta > split){
                // Can fit all split
                connectedCables[j]->right->current += split;
                sources[i]->current -= split;
            } else {
                // Split is too much
                connectedCables[j]->right->current += missingDelta;
                sources[i]->current -= missingDelta;
                // Excess energy causes split to change
                split = sources[i]->current / connectedCablesRemaining;
            }
        }
    }

    // Check if any sink is not fully filled
    bool anySinkMissingEnergy = false;
    for(int i=0;i<sinks.size();i++) {
        auto required = sinks[i]->required;
        auto delta = abs(sinks[i]->current - required);
        if(delta > 1) {
            anySinkMissingEnergy = true;
            sinks[i]->warning = true;
        }else{
            sinks[i]->warning = false;
        }
    }

    if(anySinkMissingEnergy) {
        health--;
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
        Source *source = new Source();
        source->xPos = 100;
        source->yPos = sources[sources.size() - 1]->yPos + verticalSeparation;
        source->generated = random(generatedMin, generatedMax);
        source->direction = 0;
        source->type = rand()%sourceTextures.size();
        sources.push_back(source);
    }

    if(addSink) {
        Sink *sink = new Sink();
        sink->xPos = 500;
        sink->yPos = sinks[sinks.size() - 1]->yPos + verticalSeparation;
        sink->required = random(requiredMin, requiredMax);
        sink->direction = 0;
        sink->warning = false;
        sink->type = rand()%sinkTextures.size();
        sinks.push_back(sink);
    }

    Cable c;
    c.left = sources[sources.size()-1];
    c.right = sinks[sinks.size()-1];
    c.size = 3;
    c.offset = calculateAndUpdateOffset(c.left, c.right);
    c.health = 5;
    cables.push_back(c);
}

void Game::handleMouseMotion() {
    if(mouseY > screenHeight - scrollWindow){
        mCamY = (mouseY - (screenHeight - scrollWindow)) / scrollFactor;
    } else if(mouseY < scrollWindow) {
        mCamY = -(scrollWindow - mouseY) / scrollFactor;
    } else {
        mCamY = 0;
    }
}

void Game::handleSpaceKey() {
    if(state == INTRO0) {
        state = INTRO1;
    } else if(state == INTRO1){
        state = INTRO2;
    } else if(state == INTRO2){
        state = INTRO3;
    } else if(state == INTRO3){
        state = PLAYING;
        levelInit();
    } else if(state == GAMEOVER){
        state = INTRO0;
    }
}
