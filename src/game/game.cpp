//
// Created by dgmuller on 6/7/25.
//

#include "../../include/game/game.hpp"
#include "../../include/ui/menu.hpp"

farcical::game::Game::Game(Engine& engine): engine{engine} {
}

std::optional<farcical::Error> farcical::game::Game::Init() {
    TransitionToState(State::ID::MainMenu);
    return std::nullopt;
}

std::optional<farcical::Error> farcical::game::Game::Update() {
    return std::nullopt;
}

void farcical::game::Game::TransitionToState(State::ID stateID) {
    switch(stateID) {
        case State::ID::MainMenu: {
            ui::Menu* menu{engine.GetUIManager().CreateMenu(Game::mainMenuID, engine.GetResourceManager())};
            const sf::Vector2u windowSize{engine.GetWindow().getSize()};
            menu->SetSize(windowSize);
            const float buttonSpacing{64.0f};
            ui::MenuItem* newGame{menu->CreateMenuItem(Game::newGameText)};
            ui::MenuItem* loadGame{menu->CreateMenuItem(Game::loadGameText)};
            ui::MenuItem* options{menu->CreateMenuItem(Game::optionsText)};
            ui::MenuItem* quitGame{menu->CreateMenuItem(Game::quitGameText)};
        }
        break;
        case State::ID::StartNewGame: {
        }
        break;
        case State::ID::LoadSavedGame: {
        }
        break;
        case State::ID::OptionsMenu: {
        } break;
    }
}
