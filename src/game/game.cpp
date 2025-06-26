//
// Created by dgmuller on 6/7/25.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include "../../include/game/game.hpp"
#include "../../include/ui/menu.hpp"
#include "../../include/ui/decoration.hpp"

farcical::game::Game::Game(Engine& engine): engine{engine},
                                            currentScene{nullptr},
                                            currentState{State::ID::MainMenu} {
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
            ui::Scene* scene{LoadScene(Game::mainMenuPath)};
            /*
            ui::Menu* menu{engine.GetUIManager().CreateMenu(Game::mainMenuID, engine.GetResourceManager())};

            std::string backgroundID{"background"};
            ResourceID bgTextureID{backgroundID + "Texture"};
            ui::Decoration* background{engine.GetUIManager().CreateDecoration(backgroundID, bgTextureID, engine.GetResourceManager(), menu)};

            const sf::Vector2u windowSize{engine.GetWindow().getSize()};
            menu->SetSize(windowSize);
            const float buttonSpacing{64.0f};
            ui::MenuItem* newGame{menu->CreateMenuItem(Game::newGameText, Event::Type::TransitionNewGame)};
            ui::MenuItem* loadGame{menu->CreateMenuItem(Game::loadGameText, Event::Type::TransitionLoadGame)};
            ui::MenuItem* options{menu->CreateMenuItem(Game::optionsText, Event::Type::TransitionOptions)};
            ui::MenuItem* quitGame{menu->CreateMenuItem(Game::quitGameText, Event::Type::QuitGame)};
            */
        }
        break;
        case State::ID::StartNewGame: {
        }
        break;
        case State::ID::LoadSavedGame: {
        }
        break;
        case State::ID::OptionsMenu: {
        }
        break;
    }
}

farcical::ui::Scene* farcical::game::Game::LoadScene(std::string_view path) {
    ui::Scene* scene{nullptr};
    std::ifstream inputFile{std::string{path}};
    const nlohmann::json json = nlohmann::json::parse(inputFile);
    auto windowSize{engine.GetWindow().getSize()};
    if(json.contains("scene")) {
        const auto& sceneJSON{json.at("scene")};
        const auto& sceneID{sceneJSON.at("id").template get<std::string>()};
        scene = engine.GetUIManager().CreateScene(sceneID);

        if(sceneJSON.contains("decorations")) {
            const auto& decorationsJSON{sceneJSON.at("decorations")};
            for(const auto& decoration: decorationsJSON) {
                const std::string decorationID{decoration.at("id").template get<std::string>()};
                const auto& textureJSON{decoration.at("texture")};
                const std::string texturePath{textureJSON.at("path").template get<std::string>()};
                const bool textureIsRepeated{textureJSON.at("isRepeated").template get<bool>()};
                ResourceID textureID{decorationID + "Texture"};
                const auto loadTextureResult{
                    engine.GetResourceManager().LoadResource(
                        textureID,
                        Resource::Type::Texture,
                        texturePath,
                        sf::IntRect{{0, 0}, {static_cast<int>(windowSize.x), static_cast<int>(windowSize.y)}},
                        textureIsRepeated)
                };
                const auto& positionJSON{decoration.at("position")};
                sf::Vector2f position{
                    static_cast<float>(positionJSON.at("x").template get<int>()),
                    static_cast<float>(positionJSON.at("y").template get<int>())
                };
                ui::Decoration* dec = engine.GetUIManager().CreateDecoration(
                    decorationID, textureID, engine.GetResourceManager(), scene);
                sf::Texture* texture{engine.GetResourceManager().GetTexture(textureID).value()};
                dec->SetTexture(texture);
                dec->SetPosition(position);
            } // for each decoration
        } // if sceneJSON contains decorations

        if(sceneJSON.contains("menu")) {
            const auto& menuJSON{sceneJSON.at("menu")};
            const auto& menuItemsJSON{menuJSON.at("items")};
            const auto& menuAlignmentJSON{menuJSON.at("alignment")};

            const auto& menuID{menuJSON.at("id").template get<std::string>()};
            ui::Menu* menu{engine.GetUIManager().CreateMenu(menuID, engine.GetResourceManager(), scene)};
            const auto& buttonSize{menu->GetButtonTexture(ui::Button::Status::Normal)->getSize()};
            menu->SetSize(sf::Vector2u{buttonSize.x, static_cast<unsigned int>(menuItemsJSON.size()) * buttonSize.y});
            const auto& vAlignmentJSON{menuAlignmentJSON.at("vertical")};
            const auto& hAlignmentJSON{menuAlignmentJSON.at("horizontal")};
            const std::string hAlignmentString{hAlignmentJSON.template get<std::string>()};
            const std::string vAlignmentString{vAlignmentJSON.template get<std::string>()};
            if(hAlignmentString == "centered") {
                sf::Vector2f position{
                    (static_cast<float>(windowSize.x) / 2.0f) - (static_cast<float>(menu->GetSize().x) / 2.0f),
                    menu->GetPosition().y
                };
                menu->SetPosition(position);
            } // if alignment.horizontal == "centered"
            if(vAlignmentString == "centered") {
                sf::Vector2f position{
                    menu->GetPosition().x,
                    (static_cast<float>(windowSize.y) / 2.0f) - (static_cast<float>(menu->GetSize().y) / 2.0f)
                };
                menu->SetPosition(position);
            } // if alignment.vertical == "centered
            for(const auto& item: menuItemsJSON) {
                const std::string itemID{item.at("id").template get<std::string>()};
                const std::string itemLabel{item.at("label").template get<std::string>()};
                const std::string onSelectionString{item.at("onSelection").template get<std::string>()};
                Event onSelectionEvent{Event::Type::TransitionMainMenu};
                if(onSelectionString == "QuitGame") {
                    onSelectionEvent.type = Event::Type::QuitGame;
                } else if(onSelectionString == "TransitionNewGame") {
                    onSelectionEvent.type = Event::Type::TransitionNewGame;
                } else if(onSelectionString == "TransitionLoadGame") {
                    onSelectionEvent.type = Event::Type::TransitionLoadGame;
                } else if(onSelectionString == "TransitionOptions") {
                    onSelectionEvent.type = Event::Type::TransitionOptions;
                } else if(onSelectionString == "TransitionMainMenu") {
                    onSelectionEvent.type = Event::Type::TransitionMainMenu;
                }
                ui::MenuItem* menuItem{menu->CreateMenuItem(itemID, itemLabel, onSelectionEvent.type)};
            }
        }
    } // if json contains scene

    return scene;
}
