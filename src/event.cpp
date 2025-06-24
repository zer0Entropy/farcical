//
// Created by dgmuller on 6/22/25.
//
#include "../include/event.hpp"

#include "../include/game/game.hpp"

farcical::EventSystem::EventSystem(game::Game& game, Engine& engine): System(System::ID::EventSystem),
                                                                      game{game}, engine{engine} {
}

void farcical::EventSystem::Enqueue(const Event& event) {
    eventQueue.push_back(event);
}

void farcical::EventSystem::Init() {
}

void farcical::EventSystem::Update() {
    // Iterate through a copy of the eventQueue, to prevent changes to the queue from messing up our iteration
    const auto queueCopy{eventQueue};
    eventQueue.clear();
    for(const auto& event: queueCopy) {
        switch(event.type) {
            case Event::Type::QuitGame: {
                engine.Stop();
            }
            break;
            case Event::Type::TransitionMainMenu: {
                game.TransitionToState(game::Game::State::ID::MainMenu);
            }
            break;
            case Event::Type::TransitionNewGame: {
                game.TransitionToState(game::Game::State::ID::StartNewGame);
            }
            break;
            case Event::Type::TransitionLoadGame: {
                game.TransitionToState(game::Game::State::ID::LoadSavedGame);
            }
            break;
            case Event::Type::TransitionOptions: {
                game.TransitionToState(game::Game::State::ID::OptionsMenu);
            }
            break;
        }
    }
}

void farcical::EventSystem::Stop() {
    eventQueue.clear();
}
