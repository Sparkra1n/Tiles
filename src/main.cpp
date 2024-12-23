#include "Game.h"

int main(int argc, char** argv)
{
    Game game("resources/start.txt", "Player");
    game.run();
    return 0;
}
