#include "Game.h"

int main(int argc, char** argv)
{
    Game game("resources/start.csv", "player");
    game.run();
    return 0;
}
