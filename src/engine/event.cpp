//
// Created by dgmuller on 6/22/25.
//
#include "../../include/engine/event.hpp"
#include "../../include/game/game.hpp"

farcical::engine::EventSystem::EventSystem(game::Game& game, Engine& engine):
    System(ID::EventSystem),
    game{game}, engine{engine} {
}

void farcical::engine::EventSystem::RegisterHandler(Event::Type type, EventHandler* handler) {
    handlers.insert(std::make_pair(type, handler));
}

void farcical::engine::EventSystem::UnregisterHandler(Event::Type type, EventHandler* handler) {
    const auto& findHandler{handlers.find(type)};
    if(findHandler != handlers.end()) {
        handlers.erase(findHandler);
    } // if handler found
}

void farcical::engine::EventSystem::Enqueue(const Event& event) {
    eventQueue.push_back(event);
}

void farcical::engine::EventSystem::Init() {
}

void farcical::engine::EventSystem::Update() {
    // Iterate through a copy of the eventQueue, to prevent changes to the queue from messing up our iteration
    const auto queueCopy{eventQueue};
    eventQueue.clear();

    for(const auto& event: queueCopy) {
        const auto& findHandler{handlers.find(event.type)};
        if(findHandler != handlers.end()) {
            findHandler->second->HandleEvent(event);
        } // if handler found
    } // for each Event in queueCopy

    /*
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
                //game.TransitionToState(game::Game::State::ID::MainMenu);
            }
            break;
            case Event::Type::TransitionNewGame: {
                //game.TransitionToState(game::Game::State::ID::StartNewGame);
            }
            break;
            case Event::Type::TransitionLoadGame: {
                //game.TransitionToState(game::Game::State::ID::LoadSavedGame);
            }
            break;
            case Event::Type::TransitionOptions: {
                //game.TransitionToState(game::Game::State::ID::OptionsMenu);
            }
            break;
        }
    }
    */
}

void farcical::engine::EventSystem::Stop() {
    eventQueue.clear();
}
