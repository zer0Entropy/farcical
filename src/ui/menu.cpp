//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/menu.hpp"

#include "../../include/color.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/label.hpp"
#include "../../include/ui/manager.hpp"

farcical::ui::MenuItem::MenuItem(std::string_view name, Event::Type onSelection,
                                 Widget* parent): Container(name, Widget::Type::MenuItem, parent, true),
                                                  EventPropagator(dynamic_cast<EventPropagator*>(parent)),
                                                  button{nullptr},
                                                  label{nullptr},
                                                  triggeredOnSelection{onSelection} {
}

farcical::ui::Button* farcical::ui::MenuItem::CreateButton(std::string_view name, std::vector<sf::Texture*> textures) {
  children.emplace_back(std::make_unique<Button>(name, this));
  button = dynamic_cast<Button*>(children.rbegin()->get());
  button->SetTexture(Button::Status::Normal, *textures[static_cast<int>(Button::Status::Normal)]);
  button->SetTexture(Button::Status::Highlighted, *textures[static_cast<int>(Button::Status::Highlighted)]);
  button->SetTexture(Button::Status::Pressed, *textures[static_cast<int>(Button::Status::Pressed)]);
  return button;
}

farcical::ui::Label* farcical::ui::MenuItem::CreateLabel(
  std::string_view name, const TextProperties& labelProperties, sf::Font& font) {
  children.emplace_back(std::make_unique<Label>(name, this));
  label = dynamic_cast<Label*>(children.rbegin()->get());
  this->labelProperties = labelProperties;
  label->SetFont(font);
  label->SetFontSize(labelProperties.fontSize);
  label->SetFontColor(labelProperties.fontColor);
  label->SetOutlineColor(labelProperties.outlineColor);
  label->SetOutlineThickness(labelProperties.outlineThickness);
  label->SetContents(labelProperties.contents);
  return label;
}

farcical::ui::Button* farcical::ui::MenuItem::GetButton() const {
  return button;
}

farcical::ui::Label* farcical::ui::MenuItem::GetLabel() const {
  return label;
}

void farcical::ui::MenuItem::DoAction(Action action) {
  if(action.type == Action::Type::ConfirmSelection) {
    dynamic_cast<EventPropagator*>(parent)->ReceiveEvent(Event{triggeredOnSelection});
  } // if action == ConfirmSelection
  else if(action.type == Action::Type::ReceiveFocus) {
    if(button->GetStatus() == Button::Status::Normal) {
      button->SetStatus(Button::Status::Highlighted);
      label->SetOutlineColor(sf::Color::Black);
    }
  } // else if action == ReceiveFocus
  else if(action.type == Action::Type::LoseFocus) {
    if(button->GetStatus() == Button::Status::Highlighted || button->GetStatus() == Button::Status::Pressed) {
      button->SetStatus(Button::Status::Normal);
      std::string colorName{"gray"};
      label->SetOutlineColor(GetColorByName(colorName));
    }
  } // else if action == LoseFocus
  else if(action.type == Action::Type::SetPressedTrue) {
    button->SetStatus(Button::Status::Pressed);
  } // else if action == SetPressedTrue
  else if(action.type == Action::Type::SetPressedFalse) {
    button->SetStatus(Button::Status::Normal);
  } // else if action == SetPressedFalse
}


farcical::ui::Menu::Menu(std::string_view name, Manager& uiManager, Widget* parent):
  Container(name, Widget::Type::Menu, parent, false),
  EventPropagator(dynamic_cast<EventPropagator*>(&uiManager)),
  uiManager{uiManager},
  buttonTextureNormal{nullptr},
  buttonTextureHighlighted{nullptr},
  buttonTexturePressed{nullptr},
  labelFont{nullptr},
  titleFont{nullptr},
  selectedIndex{-1},
  buttonSpacing{0.0f} {
}

void farcical::ui::Menu::AddChild(std::unique_ptr<Widget> child) {
  unsigned int childIndex{GetNumChildren()};
  Container::AddChild(std::move(child));
  Widget* movedChild(GetChild(childIndex));
  if(movedChild->GetType() == Widget::Type::MenuItem) {
    items.push_back(dynamic_cast<MenuItem*>(movedChild));
  }
}

int farcical::ui::Menu::GetNumMenuItems() const {
  return items.size();
}

farcical::ui::MenuItem* farcical::ui::Menu::CreateMenuItem(
  std::string_view name, const TextProperties& labelProperties, Event::Type onSelection) {
  const int index{this->GetNumMenuItems()};
  this->AddChild(std::make_unique<MenuItem>(name, onSelection, this));
  MenuItem* item{GetMenuItemByIndex(index)};
  std::string buttonName{std::string{name} + "Button"};
  std::string labelName{std::string{name} + "Label"};
  std::vector textureList{buttonTextureNormal, buttonTextureHighlighted, buttonTexturePressed};
  Button* button{item->CreateButton(buttonName, textureList)};
  Label* label{item->CreateLabel(labelName, labelProperties, *labelFont)};

  button->SetScale(sf::Vector2f{3.0f, 3.0f});
  const sf::Vector2f buttonSize{button->GetSize()};
  const int numButtons{static_cast<int>(children.size())};

  const sf::Vector2f menuCenter{
    this->position.x + static_cast<float>(this->size.x) / 2.0f,
    this->position.y + static_cast<float>(this->size.y) / 2.0f
  };
  button->SetPosition(sf::Vector2f{
    menuCenter.x - buttonSize.x / 2.0f,
    this->position.y + (buttonSize.y * static_cast<float>(numButtons)) + (buttonSpacing * static_cast<float>(numButtons))
    //buttonSize.y * (static_cast<float>(numButtons) + 1.0f) + buttonSpacing * (static_cast<float>(numButtons) + 1.0f)
  });
  const sf::Vector2f buttonCenter{
    button->GetPosition().x + buttonSize.x / 2.0f,
    button->GetPosition().y + buttonSize.y / 2.0f
  };
  label->SetPosition(sf::Vector2f{
    buttonCenter.x - static_cast<float>(label->GetSize().x) / 2.0f,
    buttonCenter.y - static_cast<float>(label->GetSize().y) / 2.0f
  });
  return item;
}

farcical::ui::MenuItem* farcical::ui::Menu::GetMenuItemByName(std::string_view name) const {
  for(const auto& item: items) {
    if(item->GetName() == name) {
      return item;
    }
  }
  return nullptr;
}

farcical::ui::MenuItem* farcical::ui::Menu::GetMenuItemByIndex(int index) const {
  if(items.size() <= index) {
    return nullptr;
  }
  return items.at(index);
}

farcical::ui::MenuItem* farcical::ui::Menu::GetMenuItemUnderCursor(sf::Vector2i position) const {
  for(const auto& item: items) {
    const Button* button{item->GetButton()};
    if(position.x >= static_cast<int>(button->GetPosition().x)
       && position.y >= static_cast<int>(button->GetPosition().y)
       && position.x < static_cast<int>(button->GetPosition().x) + button->GetSize().x
       && position.y < static_cast<int>(button->GetPosition().y) + button->GetSize().y) {
      return item;
    }
  } // for each child
  return nullptr;
}

int farcical::ui::Menu::GetSelectedIndex() const {
  return selectedIndex;
}

sf::Texture* farcical::ui::Menu::GetButtonTexture(Button::Status state) const {
  if(state == Button::Status::Normal) {
    return buttonTextureNormal;
  } else if(state == Button::Status::Highlighted) {
    return buttonTextureHighlighted;
  } else if(state == Button::Status::Pressed) {
    return buttonTexturePressed;
  }
  return nullptr;
}

void farcical::ui::Menu::SetButtonTexture(Button::Status state, sf::Texture& texture) {
  switch(state) {
    case Button::Status::Normal: {
      buttonTextureNormal = &texture;
    }
    break;
    case Button::Status::Highlighted: {
      buttonTextureHighlighted = &texture;
    }
    break;
    case Button::Status::Pressed: {
      buttonTexturePressed = &texture;
    }
    break;
  }
}

void farcical::ui::Menu::SetLabelFont(sf::Font& font) {
  labelFont = &font;
}

void farcical::ui::Menu::SetTitleFont(sf::Font& font) {
  titleFont = &font;
}

void farcical::ui::Menu::SetSelectedIndex(int index) {
  selectedIndex = index;
}

void farcical::ui::Menu::SetButtonSpacing(float spacing) {
  buttonSpacing = spacing;
}

void farcical::ui::Menu::DoAction(Action action) {
  if(action.type == Action::Type::MoveSelectionUp) {
    --selectedIndex;
    if(selectedIndex < 0) {
      selectedIndex = static_cast<int>(children.size()) - 1;
    }
  } else if(action.type == Action::Type::MoveSelectionDown) {
    ++selectedIndex;
    if(selectedIndex >= children.size()) {
      selectedIndex = 0;
    }
  }
}
