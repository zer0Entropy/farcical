//
// Created by dgmuller on 5/24/25.
//

#include "../include/engine.hpp"
#include "../include/game/game.hpp"
#include <cassert>
#include <fstream>

farcical::Engine::Engine():
    status{farcical::Engine::Status::Uninitialized},
    window{nullptr},
    renderSystem{nullptr},
    inputSystem{nullptr},
    eventSystem{nullptr},
    uiManager{nullptr},
    game{nullptr} {
}

farcical::Engine::Status farcical::Engine::GetStatus() const {
    return status;
}

const farcical::WindowProperties& farcical::Engine::GetWindowProperties() const {
  return windowConfig;
}

std::optional<farcical::Error> farcical::Engine::Init(game::Game *game) {
  if(status == Engine::Status::Uninitialized) {
    this->game = game;
    auto result{LoadConfig(Engine::configPath)};
    if(!result.has_value()) {
      return result.error();
    }
    config = result.value();
    // Load config data from JSON
    for(auto& [key, value] : config.data.items()) {
      // Parses ConfigType, returns error if != "application"
      if(key == "type") {
        if(value != "application") {
          std::string failMsg{"Unexpected JSON value [key=" + key + "; value=" + value.template get<std::string>() + "]."};
          return Error{Error::Signal::UnexpectedValue, failMsg};
        }
        config.type = Config::Type::Application;
      }
      // Parses WindowProperties
      if(key == "window_properties") {
        for(auto& [winCfgKey, winCfgValue] : value.items()) {
          if(winCfgKey == "width") {
            windowConfig.displayMode.x = winCfgValue.template get<int>();
          }
          else if(winCfgKey == "height") {
            windowConfig.displayMode.y = winCfgValue.template get<int>();
          }
          else if(winCfgKey == "title") {
            windowConfig.title = winCfgValue.template get<std::string>();
          }
          else if(winCfgKey == "fullscreen") {
            windowConfig.fullscreen = winCfgValue.template get<bool>();
          }
          else if(winCfgKey == "position") {
            auto xIter{winCfgValue.find("x")};
            auto yIter{winCfgValue.find("y")};
            if(xIter != winCfgValue.end()) {
              windowConfig.position.x = *xIter;
            }
            if(yIter != winCfgValue.end()) {
              windowConfig.position.y = *yIter;
            }
          }
        }
      }
    }

    // Init window
    window = std::make_unique<sf::RenderWindow>();
    if(windowConfig.fullscreen) {
      window->create(sf::VideoMode{windowConfig.displayMode}, windowConfig.title, sf::State::Fullscreen);
    }
    else {
      window->create(sf::VideoMode{windowConfig.displayMode}, windowConfig.title, sf::Style::None);
      window->setPosition(windowConfig.position);
    }
    windowConfig.sizeInPixels = window->getSize();

    // Init systems
    eventSystem = std::make_unique<EventSystem>(*game, *this);
    uiManager = std::make_unique<ui::Manager>(*eventSystem);
    renderSystem = std::make_unique<RenderSystem>(*window, *uiManager);
    inputSystem = std::make_unique<InputSystem>(*window, *uiManager);

    renderSystem->Init();
    inputSystem->Init();
    eventSystem->Init();

    // Loads UI configuration from "dat/ui.json"
    uiManager->Init(resourceManager);

    status = Engine::Status::IsRunning;
  } // if Uninitialized
  return std::nullopt;
}

void farcical::Engine::Update() {
  while(const std::optional event = window->pollEvent()) {
    if(event->is<sf::Event::Closed>()) {
      window->close();
      status = Engine::Status::StoppedSuccessfully;
    }
  }
  if(status == Engine::Status::IsRunning) {
    renderSystem->Update();
    game->Update();
  }
}

void farcical::Engine::Stop() { renderSystem->Stop(); }

sf::RenderWindow& farcical::Engine::GetWindow() const {
  assert(window != nullptr && "Unexpected nullptr: application window");
  return *window;
}

farcical::ResourceManager& farcical::Engine::GetResourceManager() const {
  return const_cast<ResourceManager&>(resourceManager);
}

farcical::ui::Manager& farcical::Engine::GetUIManager() const {
  assert(uiManager && "Unexpected nullptr: uiManager");
  return *uiManager;
}

farcical::RenderSystem& farcical::Engine::GetRenderSystem() const {
  assert(renderSystem != nullptr && "Unexpected nullptr: rendersystem");
  return *renderSystem;
}
