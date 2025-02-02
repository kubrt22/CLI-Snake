#include "Utils.hpp"
#include "Game.hpp"

int main(int argc, char *argv[]) {
    argumentParser(argc, argv);
    
    Game game;
    
    game.Init();
    game.Run();
    game.Exit();
    
    return 0;
}