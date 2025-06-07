//
// Created by dgmuller on 5/30/25.
//
#include "../../include/resource/manager.hpp"

#include <fstream>
#include "../../include/ui/manager.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/menu.hpp"
#include "../../include/resource/manager.hpp"
#include "../../include/color.hpp"

farcical::ui::Manager::Manager():
  config{},
  buttonTexture{nullptr},
  buttonFont{nullptr},
  defaultFontSize{0},
  defaultFontColor{sf::Color::Black},
  defaultOutlineColor{sf::Color::Black},
  defaultOutlineThickness{0.0f} {

}

std::optional<farcical::Error> farcical::ui::Manager::Init(farcical::ResourceManager& resourceManager) {
  const std::string cfgPath{"dat/ui.json"};
  auto result{LoadConfig(cfgPath)};
  if(!result.has_value()) {
    return result.error();
  }
  config = result.value();
  for(auto& [key, value]: config.data.items()) {
    if(key == "button") {
      for(auto& [buttonKey, buttonValue]: value.items()) {
        if(buttonKey == "font") {
          ResourceID fontID;
          std::string fontPath;
          for(auto& [fontKey, fontValue]: buttonValue.items()) {
            if(fontKey == "id") {
              fontID = fontValue.template get<std::string>();
            }
            else if(fontKey == "path") {
              fontPath = fontValue.template get<std::string>();
            }
            else if(fontKey == "defaultSize") {
              defaultFontSize = fontValue.template get<int>();
            }
            else if(fontKey == "defaultColor") {
              defaultFontColor = GetColorByName(fontValue.template get<std::string>());
            }
            else if(fontKey == "defaultOutlineColor") {
              defaultOutlineColor = GetColorByName(fontValue.template get<std::string>());
            }
            else if(fontKey == "defaultOutlineThickness") {
              defaultOutlineThickness = fontValue.template get<float>();
            }
          } // for each key-value pair in font
          auto loadFontResult{resourceManager.LoadResource(fontID, Resource::Type::Font, fontPath)};
          if(loadFontResult.has_value()) {
            return loadFontResult.value();
          }
        } // if font
        else if(buttonKey == "texture") {
          constexpr unsigned int numTextures{3};
          Resource textures[numTextures];
          unsigned int textureIndex{0};
          for(auto segmentConfig: buttonValue) {
            for(auto& [segmentKey, segmentValue]: segmentConfig.items()) {

              if(segmentKey == "segment") {
                if(segmentValue == "left") {
                  textures[textureIndex].id = "buttonTextureLeft";
                } // if left
                else if(segmentValue == "center") {
                  textures[textureIndex].id = "buttonTextureCenter";
                } // else if center
                else if(segmentValue == "right") {
                  textures[textureIndex].id = "buttonTextureRight";
                } // else if right
              } // if segment

              else if(segmentKey == "path") {
                textures[textureIndex].path = segmentValue.template get<std::string>();
              } // if path
              else if(segmentKey == "position") {
                textures[textureIndex].rect.position.x = segmentValue.at("x").template get<int>();
                textures[textureIndex].rect.position.y = segmentValue.at("y").template get<int>();
              } // if position
              else if(segmentKey == "size") {
                textures[textureIndex].rect.size.x = segmentValue.at("width").template get<int>();
                textures[textureIndex].rect.size.y = segmentValue.at("height").template get<int>();
              } // if size

            } // for each key-value pair in segmentConfig
            ++textureIndex;
          } // for each segmentConfig in Button
          for(auto& texture: textures) {
            auto loadResult{resourceManager.LoadResource(texture.id, Resource::Type::Texture, texture.path, texture.rect)};
            if(loadResult.has_value()) {
              return loadResult;
            }
          }
        } // if texture
      } // for each key-value pair in Button
    } // if button
  } // for each top-level key-value pair

  const std::vector<ResourceID> textureIDList{"buttonTextureLeft", "buttonTextureCenter", "buttonTextureRight"};
  auto requestSplice(resourceManager.SpliceTextures(textureIDList, std::string{Manager::buttonTextureID}));
  if(requestSplice.has_value()) {
    return requestSplice;
  }

  return std::nullopt;
}

farcical::ui::Menu* farcical::ui::Manager::CreateMenu(std::string_view name, ResourceManager& resourceManager) {
  widgets.emplace_back(std::make_unique<Menu>(name, nullptr));
  Menu* menu{dynamic_cast<Menu*>(widgets.rbegin()->get())};
  auto textureRequest{resourceManager.GetTexture(static_cast<ResourceID>(Manager::buttonTextureID))};
  if(!textureRequest.has_value()) {
    return nullptr;
  }
  sf::Texture* buttonTexture{textureRequest.value()};
  menu->SetButtonTexture(*buttonTexture);
  auto fontRequest{resourceManager.GetFont(static_cast<ResourceID>(Manager::buttonFontID))};
  if(!fontRequest.has_value()) {
    return nullptr;
  }
  sf::Font* font{fontRequest.value()};
  menu->SetLabelFont(*font);
  return menu;
}

farcical::ui::Widget* farcical::ui::Manager::GetWidget(
    std::string_view name) const {
  for(const auto& widget : widgets) {
    if (widget->GetName() == name) {
      return widget.get();
    }
  }
  return nullptr;
}
void farcical::ui::Manager::Update(sf::RenderWindow& window) const {
  for(const auto& widget : widgets) {
    widget->Draw(window);
  }
}
/*
std::expected<farcical::ui::Config, farcical::Error> farcical::ui::Manager::LoadConfig(std::string_view path) {
  std::ifstream configFile(std::string{path}, std::ios_base::in);
  if(!configFile.good()) {
    std::string failMsg{"Failed to load config file from " + std::string{path} + "."};
    return std::unexpected(Error{Error::Signal::InvalidPath, failMsg});
  }
  while(configFile.good()) {
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
  }
}
*/