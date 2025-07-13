//
// Created by dgmuller on 5/24/25.
//

#include "../../include/engine/engine.hpp"
#include "../../include/game/game.hpp"
#include <cassert>

farcical::engine::Engine::Engine(std::string_view configPath):
    status{Status::Uninitialized},
    config{},
    window{nullptr},
    renderSystem{nullptr},
    inputSystem{nullptr},
    eventSystem{nullptr},
    game{nullptr} {

  // Create a ResourceHandle for the engine's config document
  const auto& createHandle{resourceManager.CreateResourceHandle(std::string{configDocumentID}, ResourceHandle::Type::JSONDocument, configPath)};
  if(createHandle.has_value()) {
    const ResourceHandle* jsonDocHandle{createHandle.value()};
    // Load JSON data from the document
    const auto& requestJSONDoc{resourceManager.GetJSONDoc(jsonDocHandle->id)};
    if(requestJSONDoc.has_value()) {
      // Extract configuration from JSON
      const auto& loadConfig{LoadConfig(*requestJSONDoc.value())};
      if(loadConfig.has_value()) {
        config = loadConfig.value();
      } // if loadConfig == success
    } // if requestJSONDoc == success
  } // if createHandle == success


}

farcical::engine::Engine::Status farcical::engine::Engine::GetStatus() const {
    return status;
}

const farcical::engine::Config& farcical::engine::Engine::GetConfig() const {
  return config;
}

std::optional<farcical::engine::Error> farcical::engine::Engine::Init(game::Game *game) {
  if(status == Status::Uninitialized) {
    this->game = game;

    // Initialize Window
    window = std::make_unique<sf::RenderWindow>();
    auto& windowProperties{config.windowProperties};
    if(windowProperties.fullscreen) {
      window->create(sf::VideoMode{windowProperties.displayMode}, windowProperties.title, sf::State::Fullscreen);
    }
    else {
      window->create(sf::VideoMode{windowProperties.displayMode}, windowProperties.title, sf::Style::None);
      window->setPosition(windowProperties.position);
    }
    if(!window->isOpen()) {
      const std::string failMsg{"Invalid configuration: Failed to open window."};
      return Error{Error::Signal::InvalidConfiguration, failMsg};
    }
    windowProperties.sizeInPixels = window->getSize();

    // Create Systems
    eventSystem = std::make_unique<EventSystem>(*game, *this);
    renderSystem = std::make_unique<RenderSystem>(*window);
    inputSystem = std::make_unique<InputSystem>(*window);

    // Initialize Systems
    renderSystem->Init();
    inputSystem->Init();
    eventSystem->Init();

    // Initialize Game
    game->Init();

    status = Status::IsRunning;
  } // if Uninitialized
  return std::nullopt;
}

void farcical::engine::Engine::Update() {
  while(const std::optional event = window->pollEvent()) {
    if(event->is<sf::Event::Closed>()) {
      window->close();
      status = Status::StoppedSuccessfully;
    }
  }
  if(status == Status::IsRunning) {
    renderSystem->Update();
    inputSystem->Update();
    eventSystem->Update();
    game->Update();
  }
}

void farcical::engine::Engine::Stop() {
  eventSystem->Stop();
  inputSystem->Stop();
  renderSystem->Stop();
  if(status == Status::IsRunning) {
    status = Status::StoppedSuccessfully;
  }
}

sf::RenderWindow& farcical::engine::Engine::GetWindow() const {
  assert(window != nullptr && "Unexpected nullptr: application window");
  return *window;
}

farcical::ResourceManager& farcical::engine::Engine::GetResourceManager() const {
  return const_cast<ResourceManager&>(resourceManager);
}

farcical::engine::RenderSystem& farcical::engine::Engine::GetRenderSystem() const {
  assert(renderSystem != nullptr && "Unexpected nullptr: rendersystem");
  return *renderSystem;
}

farcical::engine::InputSystem& farcical::engine::Engine::GetInputSystem() const {
  assert(inputSystem != nullptr && "Unexpected nullptr: inputsystem");
  return *inputSystem;
}

farcical::engine::EventSystem& farcical::engine::Engine::GetEventSystem() const {
  assert(eventSystem != nullptr && "Unexpected nullptr: eventsystem");
  return *eventSystem;
}
