//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/menu.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/label.hpp"
#include "../../include/ui/manager.hpp"

int farcical::ui::MenuItem::fontSize = 24;
sf::Color farcical::ui::MenuItem::fontColor = sf::Color::White;
sf::Color farcical::ui::MenuItem::outlineColor = sf::Color::Red;

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

farcical::ui::Label* farcical::ui::MenuItem::CreateLabel(std::string_view name,
                                                         sf::Font& font) {
  children.emplace_back(std::make_unique<Label>(name, this));
  label = dynamic_cast<Label*>(children.rbegin()->get());
  label->SetFont(font);
  label->SetFontSize(MenuItem::fontSize);
  label->SetFontColor(MenuItem::fontColor);
  label->SetOutlineColor(MenuItem::outlineColor);
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
    }
  } // else if action == ReceiveFocus
  else if(action.type == Action::Type::LoseFocus) {
    if(button->GetStatus() == Button::Status::Highlighted) {
      button->SetStatus(Button::Status::Normal);
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

farcical::ui::MenuItem* farcical::ui::Menu::CreateMenuItem(std::string_view name, std::string_view labelContents,
                                                           Event::Type onSelection) {
  children.emplace_back(std::make_unique<MenuItem>(name, onSelection, this));
  MenuItem* item{dynamic_cast<MenuItem*>(children.rbegin()->get())};
  std::string buttonName{std::string{name} + "Button"};

  std::vector textureList{buttonTextureNormal, buttonTextureHighlighted, buttonTexturePressed};
  Button* button{item->CreateButton(buttonName, textureList)};
  Label* label{item->CreateLabel(labelContents, *labelFont)};

  button->SetScale(sf::Vector2f{3.0f, 3.0f});
  const sf::Vector2f buttonSize{button->GetSize()};
  const int numButtons{static_cast<int>(children.size())};

  const sf::Vector2f menuCenter{
    this->position.x + static_cast<float>(this->size.x) / 2.0f,
    this->position.y + static_cast<float>(this->size.y) / 2.0f
  };
  button->SetPosition(sf::Vector2f{
    menuCenter.x - buttonSize.x / 2.0f,
    buttonSize.y * (static_cast<float>(numButtons) + 1.0f) + buttonSpacing * (static_cast<float>(numButtons) + 1.0f)
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
  for(const auto& child: children) {
    if(child->GetName() == name) {
      return dynamic_cast<MenuItem*>(child.get());
    }
  }
  return nullptr;
}

farcical::ui::MenuItem* farcical::ui::Menu::GetMenuItemByIndex(int index) const {
  if(index >= children.size()) {
    return nullptr;
  }
  return dynamic_cast<MenuItem*>(children[index].get());
}

farcical::ui::MenuItem* farcical::ui::Menu::GetMenuItemUnderCursor(sf::Vector2i position) const {
  for(const auto& child: children) {
    MenuItem* item{dynamic_cast<MenuItem*>(child.get())};
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
