
#include <cmath>
#include <string>

#include "../include/game/game.hpp"

int main()
{
    farcical::Engine engine;
    farcical::game::Game game(engine);

    // Loads engine configuration from "dat/farcical.json"
    engine.Init(&game);

    // Initializes game
    game.Init();

    while(engine.GetStatus() == farcical::Engine::Status::IsRunning) {
        engine.Update();
    };
    return 0;
}
