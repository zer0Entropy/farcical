//
// Created by dgmuller on 5/24/25.
//

#include "../../include/engine/engine.hpp"
#include "../../include/game/game.hpp"
#include <cassert>

farcical::engine::Engine::Engine(std::string_view configPath) : status{Status::Uninitialized},
                                                                config{},
                                                                configPath{configPath},
                                                                window{nullptr},
                                                                sceneManager{nullptr},
                                                                logSystem{nullptr},
                                                                renderSystem{nullptr},
                                                                inputSystem{nullptr},
                                                                eventSystem{nullptr},
                                                                game{nullptr} {
}

farcical::engine::Engine::Status farcical::engine::Engine::GetStatus() const {
  return status;
}

const farcical::engine::Config& farcical::engine::Engine::GetConfig() const {
  return config;
}

std::expected<farcical::engine::Config, farcical::engine::Error> farcical::engine::Engine::CreateConfig() {
  // If a ResourceHandle for this config document already exists, destroy it
  const ResourceHandle* jsonDocHandle{resourceManager.GetResourceHandle(std::string{configDocumentID})};
  if(jsonDocHandle != nullptr) {
    resourceManager.DestroyResourceHandle(jsonDocHandle->id, jsonDocHandle->type);
    jsonDocHandle = nullptr;
  } // if ResourceHandle already exists

  //  Create a ResourceHandle for the config document
  const auto& createConfigHandle{
    resourceManager.CreateResourceHandle(std::string{configDocumentID}, ResourceHandle::Type::JSONDocument, configPath)
  };
  if(createConfigHandle.has_value()) {
    jsonDocHandle = createConfigHandle.value();

    //  Load JSON data from the document
    const auto& requestJSONDoc{resourceManager.GetJSONDoc(jsonDocHandle->id)};
    if(requestJSONDoc.has_value()) {
      //  Extract configuration from JSON
      const auto& loadConfig{LoadConfig(*requestJSONDoc.value())};
      if(loadConfig.has_value()) {
        return loadConfig.value();
      } // if loadConfig == success
      const std::string failMsg{
        "Failed to load config file (id=" + jsonDocHandle->id + "path=" + jsonDocHandle->path + ")!"
      };
      return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
    } // if requestJSONDoc == success

    const std::string failMsg{"Error: Could not find Resource with id=" + jsonDocHandle->id + "!"};
    return std::unexpected(Error{Error::Signal::ResourceNotFound, failMsg});
  } // if createConfigHandle == success

  const std::string failMsg{
    "Failed to create ResourceHandle for JSONDocument with id=" + std::string{configDocumentID} + "!"
  };
  return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
}

std::optional<farcical::engine::Error> farcical::engine::Engine::ApplyConfig(const Config& config) {
  this->config = config;

  const auto& createLogResult{CreateLogSystem()};
  if(createLogResult.has_value()) {
    return createLogResult.value();
  } // if createLogResult == failure

  // LogSystem & ResourceManager need access to each other
  logSystem->AddResourceManager(&resourceManager);
  resourceManager.AddLogSystem(logSystem.get());

  const auto& createWindowResult{CreateWindow()};
  if(createWindowResult.has_value()) {
    return createWindowResult.value();
  } // if createWindowResult == failure

  const auto& createSystemsResult{CreateSystems()};
  if(createSystemsResult.has_value()) {
    return createSystemsResult.value();
  } // if createSystemsResult == failure

  const auto& initSystemsResult{InitSystems()};
  if(initSystemsResult.has_value()) {
    return initSystemsResult.value();
  } // if initSystemsResult == failure

  return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::engine::Engine::Init(game::Game* game) {
  if(status == Status::Uninitialized) {
    this->game = game;

    const auto& createConfigResult{CreateConfig()};
    if(!createConfigResult.has_value()) {
      return createConfigResult.error();
    } // if createConfigResult == Error

    const Config engineConfig{createConfigResult.value()};
    const auto& applyConfigResult{ApplyConfig(engineConfig)};
    if(applyConfigResult.has_value()) {
      return applyConfigResult.value();
    } // if applyConfigResult == Error

    sceneManager = std::make_unique<ui::SceneManager>(*this);
    const std::string indexPath{config.scenePath + "/index.json"};
    sceneManager->LoadResourceIndex(indexPath);

    game->Init();

    const auto& initFirstScene{
      sceneManager->SetCurrentScene(EntityID{ui::SceneManager::MainMenuSceneID})
    };
    if(!initFirstScene.has_value()) {
      return initFirstScene.error();
    } // if initFirstScene == failure

    status = Status::IsRunning;
    logSystem->AddMessage("Engine initialization completed successfully.");
    logSystem->AddMessage("");
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
      return;
    }
  }
  if(status == Status::IsRunning) {
    logSystem->Update();
    renderSystem->Update();
    inputSystem->Update();
    eventSystem->Update();
    const auto& gameUpdateResult{game->Update()};
    if(gameUpdateResult.has_value()) {
      status = Status::Error;
      Stop();
    }
    if(!window->isOpen()) {
      status = Status::StoppedSuccessfully;
      return;
    }
  }
}

void farcical::engine::Engine::Stop() {
  eventSystem->Stop();
  inputSystem->Stop();
  renderSystem->Stop();
  logSystem->Stop();
  if(status == Status::IsRunning) {
    status = Status::StoppedSuccessfully;
  }
  return;
}

sf::RenderWindow& farcical::engine::Engine::GetWindow() const {
  assert(window != nullptr && "Unexpected nullptr: application window");
  return *window;
}

farcical::ResourceManager& farcical::engine::Engine::GetResourceManager() const {
  return const_cast<ResourceManager&>(resourceManager);
}

farcical::ui::SceneManager& farcical::engine::Engine::GetSceneManager() const {
  assert(sceneManager != nullptr && "Unexpected nullptr: sceneManager");
  return *sceneManager;
}

farcical::engine::LogSystem& farcical::engine::Engine::GetLogSystem() const {
  assert(logSystem != nullptr && "Unexpected nullptr: logSystem");
  return *logSystem;
}

farcical::engine::RenderSystem& farcical::engine::Engine::GetRenderSystem() const {
  assert(renderSystem != nullptr && "Unexpected nullptr: renderSystem");
  return *renderSystem;
}

farcical::engine::InputSystem& farcical::engine::Engine::GetInputSystem() const {
  assert(inputSystem != nullptr && "Unexpected nullptr: inputSystem");
  return *inputSystem;
}

farcical::engine::EventSystem& farcical::engine::Engine::GetEventSystem() const {
  assert(eventSystem != nullptr && "Unexpected nullptr: eventSystem");
  return *eventSystem;
}

std::optional<farcical::engine::Error> farcical::engine::Engine::CreateLogSystem() {
  if(config.logPath.empty()) {
    const std::string failMsg{"Error: Missing or invalid LogPath in Engine config!"};
    return Error{Error::Signal::InvalidConfiguration, failMsg};
  } // if config.logPath is empty
  const auto& logPath{config.logPath + "/" + "log.txt"};
  if(logSystem) {
    logSystem->Stop();
    logSystem.reset();
  } // if LogSystem already exists
  logSystem = std::make_unique<LogSystem>(std::string{engineLogID}, logPath);
  return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::engine::Engine::CreateWindow() {
  window = std::make_unique<sf::RenderWindow>();
  auto& windowProperties{config.windowProperties};
  if(windowProperties.fullscreen) {
    window->create(sf::VideoMode{windowProperties.displayMode}, windowProperties.title, sf::State::Fullscreen);
  } else {
    window->create(sf::VideoMode{windowProperties.displayMode}, windowProperties.title);
  }
  if(!window->isOpen()) {
    const std::string failMsg{"Invalid configuration: Failed to open window."};
    return Error{Error::Signal::InvalidConfiguration, failMsg};
  }
  window->setPosition(windowProperties.position);
  window->setFramerateLimit(60);
  windowProperties.sizeInPixels = window->getSize();
  return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::engine::Engine::CreateSystems() {
  // Note: LogSystem was previously created in engine.ApplyConfig()
  if(eventSystem) {
    eventSystem->Stop();
    eventSystem.reset();
  } // if EventSystem already exists
  eventSystem = std::make_unique<EventSystem>(*game, *this);

  if(renderSystem) {
    renderSystem->Stop();
    renderSystem.reset();
  } // if renderSystem already exists
  renderSystem = std::make_unique<RenderSystem>(*window, *logSystem);

  if(inputSystem) {
    inputSystem->Stop();
    inputSystem.reset();
  } // if inputSystem already exists
  inputSystem = std::make_unique<InputSystem>(*window, *logSystem);
  return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::engine::Engine::InitSystems() {
  logSystem->Init();
  renderSystem->Init();
  inputSystem->Init();
  eventSystem->Init();
  return std::nullopt;
}
