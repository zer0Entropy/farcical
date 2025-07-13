//
// Created by dgmuller on 5/30/25.
//
#include "../../include/resource/manager.hpp"

#include <cctype>
#include <fstream>
#include <unistd.h>
#include "../../include/ui/manager.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/menu.hpp"
#include "../../include/ui/scene.hpp"
#include "../../include/resource/manager.hpp"
#include "../../include/color.hpp"
#include "../../include/ui/decoration.hpp"

farcical::ui::Manager::Manager(EventSystem& eventSystem): KeyboardInterface(),
                                                          MouseInterface(),
                                                          ActionHandler(),
                                                          EventPropagator(nullptr),
                                                          eventSystem{eventSystem},
                                                          config{},
                                                          buttonTextures{nullptr},
                                                          buttonFont{nullptr},
                                                          currentScene{nullptr},
                                                          focusedWidget{nullptr},
                                                          buttonTextProperties{
                                                            .contents = "",
                                                            .fontID = "",
                                                            .fontSize = 0,
                                                            .fontColor = sf::Color::White,
                                                            .outlineColor = sf::Color::Black,
                                                            .outlineThickness = 0.0f
                                                          } {
}

std::optional<farcical::Error> farcical::ui::Manager::Init(farcical::ResourceManager& resourceManager) {
  const std::string cfgPath{"dat/ui.json"};
  ResourceID configID{"uiConfig"};
  auto result{resourceManager.LoadResource(configID, Resource::Type::Config, cfgPath)};
  if(result.has_value()) {
    return result;
  }
  config = *resourceManager.GetConfig(configID).value();
  for(auto& [key, value]: config.data.items()) {
    if(key == "menu") {
      for(auto& [menuKey, menuValue]: value.items()) {
        if(menuKey == "defaultButtonSpacing") {
          defaultButtonSpacing = menuValue.template get<float>();
        } // if menuKey == defaultButtonSpacing
        else if(menuKey == "title") {
          for(auto& [titleKey, titleValue]: menuValue.items()) {
            if(titleKey == "font") {
              std::string fontPath;
              for(auto& [fontKey, fontValue]: titleValue.items()) {
                if(fontKey == "id") {
                  menuTitleProperties.fontID = fontValue.template get<std::string>();
                } else if(fontKey == "path") {
                  fontPath = fontValue.template get<std::string>();
                } else if(fontKey == "defaultSize") {
                  menuTitleProperties.fontSize = fontValue.template get<int>();
                } else if(fontKey == "defaultColor") {
                  menuTitleProperties.fontColor = GetColorByName(fontValue.template get<std::string>());
                }
                else if(fontKey == "defaultOutlineColor") {
                  menuTitleProperties.outlineColor = GetColorByName(fontValue.template get<std::string>());
                } else if(fontKey == "defaultOutlineThickness") {
                  menuTitleProperties.outlineThickness = fontValue.template get<float>();
                }
              } // for each key-value pair in font
              auto loadFontResult{
                resourceManager.LoadResource(menuTitleProperties.fontID, Resource::Type::Font,
                                             fontPath)
              };
              if(loadFontResult.has_value()) {
                return loadFontResult.value();
              }
            } // if titleKey == font
          } // for each key-value pair in title
        } // else if menuKey == title
      } // for each key-value pair in menu
    } // if menu
    else if(key == "button") {
      for(auto& [buttonKey, buttonValue]: value.items()) {
        if(buttonKey == "font") {
          std::string fontPath;
          for(auto& [fontKey, fontValue]: buttonValue.items()) {
            if(fontKey == "id") {
              buttonTextProperties.fontID = fontValue.template get<std::string>();
            }
            else if(fontKey == "path") {
              fontPath = fontValue.template get<std::string>();
            }
            else if(fontKey == "defaultSize") {
              buttonTextProperties.fontSize = fontValue.template get<int>();
            }
            else if(fontKey == "defaultColor") {
              buttonTextProperties.fontColor = GetColorByName(fontValue.template get<std::string>());
            }
            else if(fontKey == "defaultOutlineColor") {
              buttonTextProperties.outlineColor = GetColorByName(fontValue.template get<std::string>());
            }
            else if(fontKey == "defaultOutlineThickness") {
              buttonTextProperties.outlineThickness = fontValue.template get<float>();
            }
          } // for each key-value pair in font
          auto loadFontResult{resourceManager.LoadResource(buttonTextProperties.fontID, Resource::Type::Font, fontPath)};
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
                  auto loadResult{
                    resourceManager.LoadResource(segment.id, Resource::Type::Texture, segment.path, segment.rect)
                  };
                  if(loadResult.has_value()) {
                    return loadResult;
                  }
                } // for each segment in segments
              } // else if segments
            } // for each key-value pair in textureDescription
            std::string leftTextureID{currentTexture->id + "Left"};
            std::string centerTextureID{currentTexture->id + "Center"};
            std::string rightTextureID{currentTexture->id + "Right"};
            std::vector<ResourceID> textureIDs{
              leftTextureID,
              centerTextureID,
              rightTextureID
            };
            auto spliceResult{resourceManager.SpliceTextures(textureIDs, currentTexture->id)};
            if(spliceResult.has_value()) {
              return spliceResult;
            }
            if(currentTexture->id == "buttonNormalTexture") {
              buttonTextures[static_cast<int>(Button::Status::Normal)] = resourceManager.GetResource(
                currentTexture->id);
            } // if(buttonNormalTexture)
            else if(currentTexture->id == "buttonHighlightedTexture") {
              buttonTextures[static_cast<int>(Button::Status::Highlighted)] = resourceManager.GetResource(
                currentTexture->id);
            } // else if(buttonHighlightedTexture)
            else if(currentTexture->id == "buttonPressedTexture") {
              buttonTextures[static_cast<int>(Button::Status::Pressed)] = resourceManager.GetResource(
                currentTexture->id);
            } // else if(buttonPressedTexture)
          } // for each textureDescription in Button
        } // if segmentedTextures
      } // for each key-value pair in Button
    } // if button
  } // for each top-level key-value pair
  return std::nullopt;
}

void farcical::ui::Manager::SetCurrentScene(std::unique_ptr<Scene> scene) {
  currentScene = std::move(scene);
}

farcical::ui::Scene* farcical::ui::Manager::GetCurrentScene() const {
  return currentScene.get();
}



/*
farcical::ui::Scene* farcical::ui::Manager::CreateScene(std::string_view name) {
  if(rootWidget) {
    rootWidget.reset(nullptr);
  }
  rootWidget = std::make_unique<Scene>(name, nullptr);
  return dynamic_cast<Scene*>(rootWidget.get());
}
*/

/*
farcical::ui::Menu* farcical::ui::Manager::CreateMenu(std::string_view name, ResourceManager& resourceManager,
                                                      Widget* parent) {
  Menu* menu{nullptr};
  if(!parent) {
    if(rootWidget) {
      rootWidget.reset(nullptr);
    }
    rootWidget = std::make_unique<Menu>(name, *this, nullptr);
    menu = dynamic_cast<Menu*>(rootWidget.get());
  } // if(!parent)
  else if(parent->IsContainer()) {
    Container* container{dynamic_cast<Container*>(parent)};
    const int childIndex{static_cast<int>(container->GetNumChildren())};
    container->AddChild(std::make_unique<Menu>(name, *this, parent));
    menu = dynamic_cast<Menu*>(container->GetChild(childIndex));
  } // else(parent)
  if(menu) {
    SetFocusedWidget(menu);
    menu->SetButtonSpacing(defaultButtonSpacing);
    auto normalTextureRequest{resourceManager.GetTexture(static_cast<ResourceID>(Manager::buttonTextureNormalID))};
    auto highlightedTextureRequest{
      resourceManager.GetTexture(static_cast<ResourceID>(Manager::buttonTextureHighlightedID))
    };
    auto pressedTextureRequest{resourceManager.GetTexture(static_cast<ResourceID>(Manager::buttonTexturePressedID))};
    if(!normalTextureRequest.has_value() || !highlightedTextureRequest.has_value() || !pressedTextureRequest.
       has_value()) {
      return nullptr;
    }
    sf::Texture* buttonNormalTexture{normalTextureRequest.value()};
    sf::Texture* buttonHighlightedTexture{highlightedTextureRequest.value()};
    sf::Texture* buttonPressedTexture{pressedTextureRequest.value()};
    menu->SetButtonTexture(Button::Status::Normal, *buttonNormalTexture);
    menu->SetButtonTexture(Button::Status::Highlighted, *buttonHighlightedTexture);
    menu->SetButtonTexture(Button::Status::Pressed, *buttonPressedTexture);
    auto fontRequest{resourceManager.GetFont(static_cast<ResourceID>(buttonTextProperties.fontID))};
    if(!fontRequest.has_value()) {
      return nullptr;
    }
    sf::Font* font{fontRequest.value()};
    menu->SetLabelFont(*font);
  } // if(menu)
  return menu;
}
*/

/*
farcical::ui::Decoration* farcical::ui::Manager::CreateDecoration(std::string_view name, std::string_view textureID,
                                                                  ResourceManager& resourceManager, Widget* parent) {
  Decoration* decoration{nullptr};
  if(!parent) {
    if(rootWidget) {
      parent = rootWidget.get();
    }
  }
  if(parent && parent->IsContainer()) {
    Container* container{dynamic_cast<Container*>(parent)};
    unsigned int childIndex{container->GetNumChildren()};
    container->AddChild(std::make_unique<Decoration>(name, parent));
    decoration = dynamic_cast<Decoration*>(container->GetChild(childIndex));
    sf::Texture* texture{resourceManager.GetTexture(std::string{textureID}).value()};
    decoration->SetTexture(texture);
  }
  return decoration;
}

farcical::ui::Label* farcical::ui::Manager::CreateFloatingText(
  std::string_view name,
  const TextProperties& properties,
  ResourceManager& resourceManager,
  Widget* parent) {
  Label* label{nullptr};
  if(!parent) {
    if(rootWidget) {
      parent = rootWidget.get();
    }
  }
  if(parent && parent->IsContainer()) {
    Container* container{dynamic_cast<Container*>(parent)};
    unsigned int childIndex{container->GetNumChildren()};
    container->AddChild(std::make_unique<Label>(name, parent));
    label = dynamic_cast<Label*>(container->GetChild(childIndex));
    sf::Font* font{resourceManager.GetFont(properties.fontID).value()};
    label->SetFont(*font);
    label->SetContents(properties.contents);
    label->SetFontSize(properties.fontSize);
    label->SetFontColor(properties.fontColor);
    label->SetOutlineColor(properties.outlineColor);
    label->SetOutlineThickness(properties.outlineThickness);
  }
  return label;
}
*/
/*
farcical::ui::Widget* farcical::ui::Manager::GetRootWidget() const {
  return rootWidget.get();
}
*/
farcical::ui::Widget* farcical::ui::Manager::GetFocusedWidget() const {
  return focusedWidget;
}

farcical::ui::Widget* farcical::ui::Manager::FindWidgetByName(std::string_view name, Widget* parent) const {
  if(!parent) {
    if(!currentScene) {
      return nullptr;
    }
    parent = currentScene.get();
  }
  if(parent->GetName() == name) {
    return parent;
  }
  if(parent->IsContainer()) {
    Container* container{dynamic_cast<Container*>(parent)};
    Widget* widget{nullptr};
    for(int n = 0; n < container->GetNumChildren(); ++n) {
      Widget* child{container->GetChild(n)};
      widget = FindWidgetByName(name, child);
      if(widget) {
        return widget;
      } // if widget found
    } // for each child in parent
  } // else recurse
  return nullptr;
}

farcical::ui::Widget* farcical::ui::Manager::FindWidgetByType(Widget::Type type, Widget* parent) const {
  if(!parent) {
    if(!currentScene) {
      return nullptr;
    }
    parent = currentScene.get();
  }
  if(parent->GetType() == type) {
    return parent;
  }
  if(parent->IsContainer()) {
    Container* container{dynamic_cast<Container*>(parent)};
    Widget* widget{nullptr};
    for(int n = 0; n < container->GetNumChildren(); ++n) {
      Widget* child{container->GetChild(n)};
      widget = FindWidgetByType(type, child);
      if(widget) {
        return widget;
      } // if widget found
    } // for each child in parent
  } // else recurse
  return nullptr;
}


void farcical::ui::Manager::SetFocusedWidget(Widget* widget) {
  if(focusedWidget && focusedWidget != widget) {
    focusedWidget->DoAction(Action{Action::Type::LoseFocus});
  }
  if(widget && widget->CanReceiveFocus()) {
    if(focusedWidget != widget) {
      focusedWidget = widget;
      focusedWidget->DoAction(Action{Action::Type::ReceiveFocus});
    }
  } else {
    focusedWidget = nullptr;
  }
}

void farcical::ui::Manager::AddLayout(std::string_view id, const Layout& layout) {
  layouts.insert(std::make_pair(std::string{id}, layout));

}

void farcical::ui::Manager::RemoveLayout(std::string_view id) {
  const auto& layoutIter{layouts.find(std::string{id})};
  if(layoutIter != layouts.end()) {
    layouts.erase(layoutIter);
  }
}

const farcical::ui::Layout farcical::ui::Manager::GetLayout(std::string_view id) const {
  const auto& layoutIter{layouts.find(std::string{id})};
  if(layoutIter != layouts.end()) {
    return layoutIter->second;
  }
  return Layout{.layer = Layout::Layer::Background, .padding = {0.0f, 0.0f, 0.0f ,0.0f}};
}

void farcical::ui::Manager::AddToLayer(Widget* widget, Layout::Layer layerID) {
  layers.at(static_cast<int>(layerID)).push_back(widget);
}

void farcical::ui::Manager::RemoveFromLayer(Widget* widget, Layout::Layer layerID) {
  auto& layer{layers.at(static_cast<int>(layerID))};
  for(auto layerIter = layer.begin(); layerIter != layer.end(); ++layerIter) {
    if((*layerIter)->GetName() == widget->GetName()) {
      layer.erase(layerIter);
      return;
    }
  }

}

const farcical::ui::TextProperties& farcical::ui::Manager::GetMenuTitleProperties() const {
  return menuTitleProperties;
}

const farcical::ui::TextProperties& farcical::ui::Manager::GetButtonTextProperties() const {
  return buttonTextProperties;
}

void farcical::ui::Manager::Update(sf::RenderWindow& window) const {
  currentScene->Draw(window);
}

void farcical::ui::Manager::ReceiveKeyboardInput(sf::Keyboard::Key input) {
  Menu* menu{dynamic_cast<Menu*>(FindWidgetByType(Widget::Type::Menu))};
  if(input == sf::Keyboard::Key::Escape) {
    eventSystem.Enqueue(Event{Event::Type::QuitGame});
  } // if(input == Escape)
  else if(input == sf::Keyboard::Key::Up) {
    if(menu) {
      menu->DoAction(Action{Action::Type::MoveSelectionUp});
      SetFocusedWidget(menu->GetMenuItemByIndex(menu->GetSelectedIndex()));
    }
  } // else if(input == Up)
  else if(input == sf::Keyboard::Key::Down) {
    if(menu) {
      menu->DoAction(Action{Action::Type::MoveSelectionDown});
      SetFocusedWidget(menu->GetMenuItemByIndex(menu->GetSelectedIndex()));
    }
  } // else if(input == Down)
  else if(input == sf::Keyboard::Key::Left) {
  } // else if(input == Left)
  else if(input == sf::Keyboard::Key::Right) {
  } // else if(input == Right)
  else if(input == sf::Keyboard::Key::Enter) {
    if(focusedWidget) {
      focusedWidget->DoAction(Action{Action::Type::SetPressedTrue});
      focusedWidget->DoAction(Action{Action::Type::ConfirmSelection});
      focusedWidget->DoAction(Action{Action::Type::SetPressedFalse});
      focusedWidget->DoAction(Action{Action::Type::ReceiveFocus});
    } // if focusedWidget
  } // else if(input == Enter)
}

void farcical::ui::Manager::ReceiveMouseMovement(sf::Vector2i position) {
  Menu* menu = dynamic_cast<Menu*>(FindWidgetByType(Widget::Type::Menu));
  if(!menu) {
    return;
  }
  // Check if any of the menu's items are at the cursor's current position
  MenuItem* hoverItem{menu->GetMenuItemUnderCursor(position)};
  if(hoverItem) {
    SetFocusedWidget(hoverItem);
  } // if hoverItem
  else {
    if(focusedWidget) {
      focusedWidget->DoAction(Action{Action::Type::LoseFocus});
    }
    SetFocusedWidget(nullptr);
  } // else !hoverItem
}

void farcical::ui::Manager::ReceiveMouseButtonPress(sf::Mouse::Button button, sf::Vector2i position) {
  Menu* menu{dynamic_cast<Menu*>(FindWidgetByType(Widget::Type::Menu))};
  if(!menu) {
    return;
  }
  MenuItem* hoverItem{menu->GetMenuItemUnderCursor(position)};
  for(int n = 0; n < menu->GetNumChildren(); ++n) {
    MenuItem* menuItem{menu->GetMenuItemByIndex(n)};
    if(menuItem == hoverItem) {
      menuItem->DoAction(Action{Action::Type::SetPressedTrue});
    } else {
      menuItem->DoAction(Action{Action::Type::SetPressedFalse});
    }
  } // for each menuItem in menu
}

void farcical::ui::Manager::ReceiveMouseButtonRelease(sf::Mouse::Button button, sf::Vector2i position) {
  Menu* menu{dynamic_cast<Menu*>(FindWidgetByType(Widget::Type::Menu))};
  if(!menu) {
    return;
  }
  MenuItem* hoverItem{menu->GetMenuItemUnderCursor(position)};
  for(int n = 0; n < menu->GetNumChildren(); ++n) {
    MenuItem* menuItem{menu->GetMenuItemByIndex(n)};
    if(menuItem == hoverItem && menuItem->GetButton()->GetStatus() == Button::Status::Pressed) {
      menuItem->DoAction(Action{Action::Type::ConfirmSelection});
    }
    menuItem->DoAction(Action{Action::Type::SetPressedFalse});
  } // for each menuItem in menu
}

void farcical::ui::Manager::DoAction(Action action) {
}

void farcical::ui::Manager::ReceiveEvent(const farcical::Event& event) {
  eventSystem.Enqueue(event);
}