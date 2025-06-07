//
// Created by dgmuller on 5/24/25.
//

#include "../include/app.hpp"
#include <cassert>
#include <fstream>

farcical::Application::Application():
    status{farcical::Application::Status::Uninitialized},
    window{nullptr},
    renderSystem{nullptr} {

}

farcical::Application::Status farcical::Application::GetStatus() const {
    return status;
}

const farcical::WindowProperties& farcical::Application::GetWindowProperties() const {
  return windowConfig;
}

std::optional<farcical::Error> farcical::Application::Init() {
  if(status == Application::Status::Uninitialized) {
    const std::string cfgPath{"dat/farcical.json"};
    auto result{LoadConfig(cfgPath)};
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


    window = std::make_unique<sf::RenderWindow>();

    if(windowConfig.fullscreen) {
      window->create(sf::VideoMode{windowConfig.displayMode}, windowConfig.title, sf::State::Fullscreen);
    }
    else {
      window->create(sf::VideoMode{windowConfig.displayMode}, windowConfig.title, sf::Style::None);
      window->setPosition(windowConfig.position);
    }

    windowConfig.sizeInPixels = window->getSize();
    renderSystem = std::make_unique<RenderSystem>(*window, uiManager);
    renderSystem->Init();

    uiManager.Init(resourceManager);

    status = Application::Status::IsRunning;
  }
  return std::nullopt;
}

void farcical::Application::Update() {
  while(const std::optional event = window->pollEvent()) {
    if(event->is<sf::Event::Closed>()) {
      window->close();
      status = Application::Status::StoppedSuccessfully;
    }
  }
  if(status == Application::Status::IsRunning) {
    renderSystem->Update();
  }
}

void farcical::Application::Stop() { renderSystem->Stop(); }

sf::RenderWindow& farcical::Application::GetWindow() const {
  assert(window != nullptr && "Unexpected nullptr: application window");
  return *window;
}

farcical::ResourceManager & farcical::Application::GetResourceManager() const {
  return const_cast<ResourceManager&>(resourceManager);
}

farcical::ui::Manager& farcical::Application::GetUIManager() const {
  return const_cast<farcical::ui::Manager&>(uiManager);
}

farcical::RenderSystem& farcical::Application::GetRenderSystem() const {
  assert(renderSystem != nullptr && "Unexpected nullptr: application rendersystem");
  return *renderSystem;
}

/*
std::expected<farcical::Application::Config, farcical::Error> farcical::Application::LoadConfig(std::string_view path) {
  std::ifstream cfgInput{std::string{path}, std::ios_base::in};
  if (!cfgInput.good()) {
    std::string failMsg{"Failed to load config file from " + std::string{path} + "."};
    return std::unexpected{Error{Error::Signal::InvalidPath, failMsg}};
  }
  while(cfgInput.good())
  {
    std::string line;
    std::getline(cfgInput, line);
    if(line.empty()) {
      break;
    }
    std::string varName{ExtractVariableName(line)};
    std::string value{ExtractValue(line)};
    if (value.find('"') != std::string::npos) {
      value = StripQuotes(value);
    }

    if(varName == "window_width") {
      config.windowCfg.displayMode.x = std::stoi(value);
    }
    else if(varName == "window_height") {
      config.windowCfg.displayMode.y = std::stoi(value);
    }
    else if(varName == "window_title") {
      config.windowCfg.title = value;
    }
    else if(varName == "window_position_x") {
      config.windowCfg.position.x = std::stoi(value);
    }
    else if(varName == "window_position_y") {
      config.windowCfg.position.y = std::stoi(value);
    }
    else if(varName == "window_fullscreen") {
      if(value == "true") {
        config.windowCfg.fullscreen = true;
      }
      else {
        config.windowCfg.fullscreen = false;
      }
    }
  }
  if (cfgInput.is_open()) {
    cfgInput.close();
  }
  return config;
}
*/
