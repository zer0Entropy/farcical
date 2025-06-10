//
// Created by dgmuller on 6/7/25.
//

#include "../../include/game/game.hpp"
#include "../../include/ui/menu.hpp"

farcical::game::Game::Game(Engine &engine): engine{engine} {
}

std::optional<farcical::Error> farcical::game::Game::Init() {
    ui::Menu *menu{engine.GetUIManager().CreateMenu(Game::mainMenuID, engine.GetResourceManager())};
    const sf::Vector2u windowSize{engine.GetWindow().getSize()};
    menu->SetSize(windowSize);
    const float buttonSpacing{64.0f};
    ui::MenuItem* newGame{menu->CreateMenuItem(Game::newGameText, buttonSpacing)};
    ui::MenuItem* loadGame{menu->CreateMenuItem(Game::loadGameText, buttonSpacing)};
    ui::MenuItem* options{menu->CreateMenuItem(Game::optionsText, buttonSpacing)};
    ui::MenuItem* quitGame{menu->CreateMenuItem(Game::quitGameText, buttonSpacing)};

    return std::nullopt;
}

std::optional<farcical::Error> farcical::game::Game::Update() {
    return std::nullopt;
}
