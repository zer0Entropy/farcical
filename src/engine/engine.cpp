//
// Created by dgmuller on 5/24/25.
//

#include "../../include/engine/engine.hpp"
#include "../../include/game/game.hpp"
#include <cassert>

farcical::engine::Engine::Engine(std::string_view configPath): status{Status::Uninitialized},
                                                               config{},
                                                               window{nullptr},
                                                               renderSystem{nullptr},
                                                               inputSystem{nullptr},
                                                               eventSystem{nullptr},
                                                               game{nullptr} {
  // Create a ResourceHandle for the engine's config document
  const auto& createHandle{
    resourceManager.CreateResourceHandle(std::string{configDocumentID}, ResourceHandle::Type::JSONDocument, configPath)
  };
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

std::optional<farcical::engine::Error> farcical::engine::Engine::Init(game::Game* game) {
  if(status == Status::Uninitialized) {
    this->game = game;

    // Initialize Window
    window = std::make_unique<sf::RenderWindow>();
    auto& windowProperties{config.windowProperties};
    if(windowProperties.fullscreen) {
      window->create(sf::VideoMode{windowProperties.displayMode}, windowProperties.title, sf::State::Fullscreen);
    } else {
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

    // Compile a ResourceList for Game
    const std::string indexPath{config.scenePath + "/index.json"};
    ResourceID sceneIndexID{static_cast<ResourceID>(sceneIndexDocumentID)};
    const auto& createHandle{
      resourceManager.CreateResourceHandle(sceneIndexID, ResourceHandle::Type::JSONDocument, indexPath)
    };
    if(createHandle.has_value()) {
      const auto& loadSceneIndex{
        resourceManager.GetJSONDoc(sceneIndexID)
      };
      if(loadSceneIndex.has_value()) {
        const auto& indexJSON{*loadSceneIndex.value()};
        ResourceList sceneResourceList;
        for(const auto& sceneResourceJSON: indexJSON) {
          const auto& findID{sceneResourceJSON.find("id")};
          const auto& findResource{sceneResourceJSON.find("resource")};
          engine::EntityID sceneID{""};
          ResourceParameters resourceParameters;
          if(findID != sceneResourceJSON.end()) {
            sceneID = findID.value().get<std::string>();
          } // if id found
          if(findResource != sceneResourceJSON.end()) {
            const auto& resourceJSON{findResource.value()};
            const auto& findResourceID{resourceJSON.find("id")};
            const auto& findResourcePath{resourceJSON.find("path")};
            if(findResourceID != resourceJSON.end()) {
              resourceParameters.first = findResourceID.value().get<std::string>();
            } // if resourceID found
            if(findResourcePath != resourceJSON.end()) {
              resourceParameters.second = config.scenePath + "/" + findResourcePath.value().get<std::string>();
            } // if resourcePath found
          } // if resource found
          sceneResourceList.push_back(std::make_pair(sceneID, resourceParameters));
        } // for each sceneResource in indexJSON
        // Initialize Game
        game->Init(sceneResourceList);
      } // if loadSceneIndex == success
    } // if createHandle == success

    status = Status::IsRunning;
  } // if Uninitialized
  return std::nullopt;
}

void farcical::engine::Engine::Update() {
  game::Game::Status gameStatus{game->GetStatus()};
  if(gameStatus == game::Game::Status::StoppedSuccessfully) {
    window->close();
    status = Status::StoppedSuccessfully;
    return;
  } // Game StoppedSuccessfully
  if(gameStatus == game::Game::Status::Error || gameStatus == game::Game::Status::Uninitialized) {
    window->close();
    status = Status::Error;
    return;
  } // Game Error || Uninitialized
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
