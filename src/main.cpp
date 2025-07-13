
#include <fstream>
#include "../include/game/game.hpp"

int main()
{
    const std::string configPath{"dat/farcical.json"};
    farcical::engine::Engine engine(configPath);
    farcical::game::Game game(engine);

    engine.Init(&game);

    while(engine.GetStatus() == farcical::engine::Engine::Status::IsRunning) {
        engine.Update();
    };
    return 0;
}
