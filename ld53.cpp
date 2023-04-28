#include "Game.h"

int main(int, char **) {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
    return game.run();
}
