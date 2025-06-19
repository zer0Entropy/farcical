//
// Created by dgmuller on 5/30/25.
//
#include "../../include/resource/manager.hpp"

#include <cctype>
#include <fstream>
#include "../../include/ui/manager.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/menu.hpp"
#include "../../include/resource/manager.hpp"
#include "../../include/color.hpp"

farcical::ui::Manager::Manager(): KeyboardInterface(),
  config{},
  buttonTexture{nullptr},
  buttonFont{nullptr},
  focusedWidget{nullptr},
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
    if(key == "menu") {
      for(auto& [menuKey, menuValue]: value.items()) {
        if(menuKey == "defaultButtonSpacing") {
          defaultButtonSpacing = menuValue.template get<float>();
        }
      } // for each key-value pair in menu
    } // if menu
    else if(key == "button") {
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
        else if(buttonKey == "segmentedTextures") {
          std::vector<Resource> segmentedTextures;
          Resource* currentTexture{nullptr};
          for(auto textureDescription: buttonValue) {
            segmentedTextures.push_back(Resource{});
            currentTexture = &segmentedTextures.back();
            for(auto& [textureKey, textureValue]: textureDescription.items()) {
              if(textureKey == "id") {
                currentTexture->id = textureValue.template get<std::string>();
                currentTexture->id.append("Texture");
              } // if textureID
              else if(textureKey == "path") {
                currentTexture->path = textureValue.template get<std::string>();
              } // else if path
              else if(textureKey == "segments") {
                // We're assuming our segments are a set of 3: left, center, right
                constexpr unsigned int numSegments{3};
                Resource segments[numSegments];
                unsigned int segmentIndex{0};
                for(auto& segmentDescription: textureValue) {
                  for(auto& [segmentKey, segmentValue]: segmentDescription.items()) {
                    if(segmentKey == "id") {
                      segments[segmentIndex].id = currentTexture->id;
                      std::string idString{segmentValue.template get<std::string>()};
                      idString[0] = static_cast<char>(std::toupper(idString[0]));
                      segments[segmentIndex].id.append(idString);
                    } // if segmentID
                    else if(segmentKey == "position") {
                      segments[segmentIndex].rect.position.x = segmentValue.at("x").template get<int>();
                      segments[segmentIndex].rect.position.y = segmentValue.at("y").template get<int>();
                    } // else if position
                    else if(segmentKey == "size") {
                      segments[segmentIndex].rect.size.x = segmentValue.at("width").template get<int>();
                      segments[segmentIndex].rect.size.y = segmentValue.at("height").template get<int>();
                    } // else if size
                  } // for each key-value pair in segmentDescription
                  segments[segmentIndex].path = currentTexture->path;
                  ++segmentIndex;
                } // for each segmentDescription in texture
                for(auto& segment: segments) {
                  auto loadResult{resourceManager.LoadResource(segment.id, Resource::Type::Texture, segment.path, segment.rect)};
                  if(loadResult.has_value()) {
                    return loadResult;
                  }
                } // for each segment in segments
              } // else if segments

            } // for each key-value pair in textureDescription
          } // for each textureDescription in Button
          for(auto& texture: segmentedTextures) {
            std::string leftTextureID{texture.id + "Left"};
            std::string centerTextureID{texture.id + "Center"};
            std::string rightTextureID{texture.id + "Right"};
            std::vector<ResourceID> textureIDs{
              static_cast<ResourceID>(leftTextureID),
              static_cast<ResourceID>(centerTextureID),
              static_cast<ResourceID>(rightTextureID)
            };
            auto spliceResult{resourceManager.SpliceTextures(textureIDs, texture.id)};
            if(spliceResult.has_value()) {
              return spliceResult;
            }
          } // for each texture in textures
        } // if segmentedTextures
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
  SetFocusedWidget(menu);
  menu->SetButtonSpacing(defaultButtonSpacing);
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

farcical::ui::Widget* farcical::ui::Manager::GetFocusedWidget() const {
  return focusedWidget;
}

void farcical::ui::Manager::SetFocusedWidget(Widget* widget) {
  focusedWidget = widget;
}

void farcical::ui::Manager::Update(sf::RenderWindow& window) const {
  for(const auto& widget : widgets) {
    widget->Draw(window);
  }
}

void farcical::ui::Manager::ReceiveKeyboardInput(sf::Keyboard::Key input) {
  if(input == sf::Keyboard::Key::Escape) {

  } // if(input == Escape)
  else if(input == sf::Keyboard::Key::Up) {

  } // else if(input == Up)
  else if(input == sf::Keyboard::Key::Down) {

  } // else if(input == Down)
  else if(input == sf::Keyboard::Key::Left) {

  } // else if(input == Left)
  else if(input == sf::Keyboard::Key::Right) {

  } // else if(input == Right)
}
