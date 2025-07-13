//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/menu.hpp"
#include "../../include/color.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/label.hpp"
#include "../../include/engine/render.hpp"

farcical::ui::MenuItem::MenuItem(engine::EntityID id, engine::Event::Type onSelection,
                                 Widget* parent): Container(id, Type::MenuItem, parent, true),
                                                  button{nullptr},
                                                  label{nullptr},
                                                  triggeredOnSelection{onSelection} {
}

farcical::ui::Button* farcical::ui::MenuItem::CreateButton(engine::EntityID id, std::vector<sf::Texture*> textures) {
  children.emplace_back(std::make_unique<Button>(id, this));
  button = dynamic_cast<Button*>(children.rbegin()->get());
  button->SetTexture(Button::Status::Normal, *textures[static_cast<int>(Button::Status::Normal)]);
  button->SetTexture(Button::Status::Highlighted, *textures[static_cast<int>(Button::Status::Highlighted)]);
  button->SetTexture(Button::Status::Pressed, *textures[static_cast<int>(Button::Status::Pressed)]);
  return button;
}

farcical::ui::Label* farcical::ui::MenuItem::CreateLabel(
  engine::EntityID id, std::string_view contents, const FontProperties& fontProperties, sf::Font& font) {
  children.emplace_back(std::make_unique<Label>(id, this));
  label = dynamic_cast<Label*>(children.rbegin()->get());
  label->SetFont(font);
  label->SetFontSize(fontProperties.characterSize);
  label->SetFontColor(fontProperties.color);
  label->SetOutlineColor(fontProperties.outlineColor);
  label->SetOutlineThickness(fontProperties.outlineThickness);
  label->SetContents(contents);
  return label;
}

farcical::engine::Event::Type farcical::ui::MenuItem::OnSelection() const {
    return triggeredOnSelection;
}

farcical::ui::Button* farcical::ui::MenuItem::GetButton() const {
  return button;
}

farcical::ui::Label* farcical::ui::MenuItem::GetLabel() const {
  return label;
}

void farcical::ui::MenuItem::DoAction(Action action) {
  engine::RenderComponent* labelRenderCmp{dynamic_cast<engine::RenderComponent*>(label->GetComponent(engine::Component::Type::Render))};
  if(action.type == Action::Type::ReceiveFocus) {
    if(button->GetStatus() == Button::Status::Normal) {
      button->SetStatus(Button::Status::Highlighted);
      label->SetOutlineColor(sf::Color::Black);
      labelRenderCmp->fontProperties.outlineColor = sf::Color::Black;
    } // if buttonStatus == Normal
  } // else if action == ReceiveFocus
  else if(action.type == Action::Type::LoseFocus) {
    if(button->GetStatus() == Button::Status::Highlighted || button->GetStatus() == Button::Status::Pressed) {
      button->SetStatus(Button::Status::Normal);
      const std::string colorName{"darkGray"};
      const sf::Color color{GetColorByName(colorName)};
      label->SetOutlineColor(color);
      labelRenderCmp->fontProperties.outlineColor = color;
    } // if buttonStatus == Highlighted || buttonStatus == Pressed
  } // else if action == LoseFocus
  else if(action.type == Action::Type::SetPressedTrue) {
    button->SetStatus(Button::Status::Pressed);
  } // else if action == SetPressedTrue
  else if(action.type == Action::Type::SetPressedFalse) {
    button->SetStatus(Button::Status::Normal);
  } // else if action == SetPressedFalse
}


farcical::ui::Menu::Menu(engine::EntityID id, Widget* parent):
  Container(id, Type::Menu, parent, false),
  buttonTextureNormal{nullptr},
  buttonTextureHighlighted{nullptr},
  buttonTexturePressed{nullptr},
  labelFont{nullptr},
  titleFont{nullptr},
  selectedIndex{-1},
  buttonSpacing{0.0f} {
}

void farcical::ui::Menu::AddChild(std::unique_ptr<Widget> child) {
  const unsigned int childIndex{GetNumChildren()};
  Container::AddChild(std::move(child));
  Widget* movedChild(GetChild(childIndex));
  if(movedChild->GetType() == Type::MenuItem) {
    items.push_back(dynamic_cast<MenuItem*>(movedChild));
  }
}

int farcical::ui::Menu::GetNumMenuItems() const {
  return static_cast<int>(items.size());
}

farcical::ui::MenuItem* farcical::ui::Menu::GetMenuItemByID(engine::EntityID id) const {
  for(const auto& item: items) {
    if(item->GetID() == id) {
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

farcical::ui::MenuController::MenuController(Menu* menu, engine::EventSystem& eventSystem): MouseInterface(),
  KeyboardInterface(),
  menu{menu}, eventSystem{eventSystem} {
}

void farcical::ui::MenuController::ReceiveMouseMovement(sf::Vector2i position) {
  MenuItem* menuItemUnderCursor{menu->GetMenuItemUnderCursor(position)};
  int selectedIndex{menu->GetSelectedIndex()};

  for(int index = 0; index < menu->GetNumMenuItems(); ++index) {
    MenuItem* menuItem{menu->GetMenuItemByIndex(index)};
    if(menuItem == menuItemUnderCursor) {
      if(selectedIndex != index) {
        menu->SetSelectedIndex(index);
        menuItem->DoAction(Action{Action::Type::ReceiveFocus});
      } // if not currently focused
    } // if this item is under the cursor
    else {
      menuItem->DoAction(Action{Action::Type::LoseFocus});
    } // else if this item is not under the cursor
  } // for each MenuItem in Menu

  if(menuItemUnderCursor == nullptr) {
    menu->SetSelectedIndex(-1);
  } // if no MenuItem is under the cursor
}

void farcical::ui::MenuController::ReceiveMouseButtonPress(sf::Mouse::Button button, sf::Vector2i position) {
  MenuItem* menuItemUnderCursor{menu->GetMenuItemUnderCursor(position)};
  int selectedIndex{menu->GetSelectedIndex()};

  for(int index = 0; index < menu->GetNumMenuItems(); ++index) {
    MenuItem* menuItem{menu->GetMenuItemByIndex(index)};
    if(menuItem == menuItemUnderCursor) {
      menu->SetSelectedIndex(index);
      menuItem->DoAction(Action{Action::Type::SetPressedTrue});
    } // if this item is under the cursor
    else {
      menuItem->DoAction(Action{Action::Type::LoseFocus});
    }
  } // for each MenuItem in Menu
}

void farcical::ui::MenuController::ReceiveMouseButtonRelease(sf::Mouse::Button button, sf::Vector2i position) {
  MenuItem* menuItemUnderCursor{menu->GetMenuItemUnderCursor(position)};
  int selectedIndex{menu->GetSelectedIndex()};

  for(int index = 0; index < menu->GetNumMenuItems(); ++index) {
    MenuItem* menuItem{menu->GetMenuItemByIndex(index)};
    if(menuItem->GetButton()->GetStatus() == Button::Status::Pressed) {
      menuItem->DoAction(Action{Action::Type::SetPressedFalse});
      if(menuItem == menuItemUnderCursor) {
        eventSystem.Enqueue(engine::Event{menuItem->OnSelection()});
      } // if this MenuItem is under the cursor, enqueue its OnSelection event
    } // if Button == Pressed
  } // for each MenuItem in Menu
  menu->SetSelectedIndex(-1);
}

void farcical::ui::MenuController::ReceiveKeyboardInput(sf::Keyboard::Key input) {
  int selectedIndex{menu->GetSelectedIndex()};
  MenuItem* previouslySelectedItem{nullptr};
  if(selectedIndex >= 0) {
    previouslySelectedItem = menu->GetMenuItemByIndex(selectedIndex);
  }

  if(input == sf::Keyboard::Key::Up) {
    if(previouslySelectedItem) {
      previouslySelectedItem->DoAction(Action{Action::Type::LoseFocus});
    } // if a MenuItem had focus prior to this input, make it lose focus
    menu->DoAction(Action{Action::Type::MoveSelectionUp});
    selectedIndex = menu->GetSelectedIndex();
    MenuItem* selectedItem{menu->GetMenuItemByIndex(selectedIndex)};
    selectedItem->DoAction(Action{Action::Type::ReceiveFocus});
  } // if input == Up

  else if(input == sf::Keyboard::Key::Down) {
    if(previouslySelectedItem) {
      previouslySelectedItem->DoAction(Action{Action::Type::LoseFocus});
    } // if a MenuItem had focus prior to this input, make it lose focus
    menu->DoAction(Action{Action::Type::MoveSelectionDown});
    selectedIndex = menu->GetSelectedIndex();
    MenuItem* selectedItem{menu->GetMenuItemByIndex(selectedIndex)};
    selectedItem->DoAction(Action{Action::Type::ReceiveFocus});
  } // else if input == Down

  else if(input == sf::Keyboard::Key::Enter) {
    if(previouslySelectedItem) {
      const engine::Event::Type eventType{previouslySelectedItem->OnSelection()};
      eventSystem.Enqueue(engine::Event{eventType});
    } // if previouslySelectedItem
  } // else if input == Enter
}
