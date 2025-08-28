//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/menu.hpp"
#include "../../include/color.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/text.hpp"
#include "../../include/engine/system/render.hpp"
#include "../../include/game/game.hpp"

farcical::ui::Menu::Menu(engine::EntityID id, Menu::Type type, const MenuLayout& layout,
                         Container* parent) : Container(id, Widget::Type::Menu, parent, false),
                                              menuType{type},
                                              layout{layout},
                                              labelFont{nullptr},
                                              titleFont{nullptr},
                                              selectedIndex{-1} {
}

void farcical::ui::Menu::AddChild(std::unique_ptr<Widget> child) {
  const unsigned int childIndex{GetNumChildren()};
  Container::AddChild(std::move(child));
  Widget* movedChild(GetChild(childIndex));
  switch(menuType) {
    case Type::Button: {
      if(movedChild->GetType() == Widget::Type::Button) {
        buttons.push_back(dynamic_cast<Button*>(movedChild));
      } // if this child is a Button
    }
    break;
    case Type::RadioButton: {
      if(movedChild->GetType() == Widget::Type::RadioButton) {
        radioButtons.push_back(dynamic_cast<RadioButton*>(movedChild));
      } // if this child is a RadioButton
    }
    break;
    case Type::SubMenu: {
      if(movedChild->GetType() == Widget::Type::Menu) {
        subMenus.push_back(dynamic_cast<Menu*>(movedChild));
      } // if this child is a Menu
    }
    break;
    default: break;
  }
}

farcical::ui::Menu::Type farcical::ui::Menu::GetMenuType() const {
  return menuType;
}

std::vector<farcical::ui::Widget*> farcical::ui::Menu::GetFocusList() const {
  std::vector<Widget*> focusList;
  if(menuType == Menu::Type::SubMenu) {
    for(const auto& subMenu: subMenus) {
      const auto& subFocusList{subMenu->GetFocusList()};
      for(const auto& focusable: subFocusList) {
        focusList.push_back(focusable);
      } // for each focusable Widget in subFocusList
    } // for each subMenu
  } // if this Menu is composed of SubMenus, concatenate their focusLists together
  else {
    for(const auto& widget: children) {
      if(widget->CanReceiveFocus()) {
        focusList.push_back(widget.get());
      } // if widget canReceiveFocus
    } // for each Widget in children
  }
  return focusList;
}

const std::vector<farcical::ui::Button*>& farcical::ui::Menu::GetButtons() const {
  return buttons;
}

const std::vector<farcical::ui::RadioButton*>& farcical::ui::Menu::GetRadioButtons() const {
  return radioButtons;
}

const std::vector<farcical::ui::Menu*>& farcical::ui::Menu::GetSubMenus() const {
  return subMenus;
}

std::vector<farcical::ui::Menu::Item> farcical::ui::Menu::GetItems() const {
  std::vector<Item> items;
  switch(menuType) {
    case Menu::Type::Button: {
      for(const auto& button: buttons) {
        items.emplace_back(button);
      } // for each Button
    }
    break;
    case Menu::Type::RadioButton: {
      for(const auto& radioButton: radioButtons) {
        items.emplace_back(radioButton);
      } // for each RadioButton
    }
    break;
    case Menu::Type::SubMenu: {
      for(const auto& subMenu: subMenus) {
        items.emplace_back(subMenu);
      } // for each subMenu
    }
    break;
  }
  return items;
}

std::optional<farcical::ui::Menu::Item> farcical::ui::Menu::GetItem(int index) const {
  const auto& items = GetItems();
  if(index >= 0 && index < items.size()) {
    return items[index];
  }
  return std::nullopt;
}

farcical::ui::Widget* farcical::ui::Menu::GetWidget(int index) const {
  Widget* widget{nullptr};
  if(menuType == Menu::Type::Button) {
    for(int i = 0; i < buttons.size(); ++i) {
      if(i == index) {
        widget = buttons[i];
        break;
      } // if i == index
    } // for each Button
  } // if Button
  else if(menuType == Menu::Type::RadioButton) {
    for(int i = 0; i < radioButtons.size(); ++i) {
      if(i == index) {
        widget = radioButtons[i];
        break;
      } // if i == index
    } // for each RadioButton
  } // else if RadioButton
  else if(menuType == Menu::Type::SubMenu) {
    for(int i = 0; i < subMenus.size(); ++i) {
      if(i == index) {
        widget = subMenus[i];
        break;
      }
    } // for each SubMenu
  } // else if SubMenu
  return widget;
}

std::optional<farcical::ui::Menu::Item> farcical::ui::Menu::GetSelectedItem() const {
  const auto& items = GetItems();
  if(selectedIndex >= 0 && selectedIndex < items.size()) {
    return items[selectedIndex];
  }
  return std::nullopt;
}

int farcical::ui::Menu::GetSelectedIndex() const {
  return selectedIndex;
}

sf::Texture* farcical::ui::Menu::GetButtonTexture(Button::Status state) const {
  const auto& findTexture{buttonTextures.find(state)};
  if(findTexture != buttonTextures.end()) {
    return findTexture->second;
  }
  return nullptr;
}

sf::Texture* farcical::ui::Menu::GetRadioButtonTexture(RadioButton::Status status) const {
  const auto& findTexture{radioButtonTextures.find(status)};
  if(findTexture != radioButtonTextures.end()) {
    return findTexture->second;
  }
  return nullptr;
}

void farcical::ui::Menu::AddButtonTexture(Button::Status status, sf::Texture& texture) {
    buttonTextures.insert(std::make_pair(status, &texture));
}

void farcical::ui::Menu::AddRadioButtonTexture(RadioButton::Status status, sf::Texture& texture) {
  radioButtonTextures.insert(std::make_pair(status, &texture));
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

void farcical::ui::Menu::SetSelectedItem(const Item& item) {
  switch(menuType) {
    case Type::Button: {
      const auto& selectButton{std::get_if<Button*>(&item)};
      if(selectButton) {
        for(int index = 0; index < buttons.size(); ++index) {
          const Button* button{buttons[index]};
          if(button == *selectButton) {
            selectedIndex = index;
            break;
          } // if this Button == selectButton
        } // for each Button
      } // if selectButton
    }
    break;
    case Type::RadioButton: {
      const auto& selectRadioButton{std::get_if<RadioButton*>(&item)};
      if(selectRadioButton) {
        for(int index = 0; index < radioButtons.size(); ++index) {
          const RadioButton* radioButton{radioButtons[index]};
          if(radioButton == *selectRadioButton) {
            selectedIndex = index;
            break;
          }
        } // for each RadioButton
      } // if selectRadioButton
    } break;
    case Type::SubMenu: {
    } break;
  }
}

void farcical::ui::Menu::DoAction(Action action) {
  int numItems{0};
  if(menuType == Menu::Type::SubMenu) {
    numItems = subMenus.size();
  } // if subMenu
  else if(menuType == Menu::Type::Button) {
    numItems = buttons.size();
  } // else if button
  else if(menuType == Menu::Type::RadioButton) {
    numItems = radioButtons.size();
  } // else if radioButton
  if(action.type == Action::Type::MoveSelectionUp) {
    --selectedIndex;
    if(selectedIndex < 0) {
      selectedIndex = static_cast<int>(numItems) - 1;
    }
  } else if(action.type == Action::Type::MoveSelectionDown) {
    ++selectedIndex;
    if(selectedIndex >= numItems) {
      selectedIndex = 0;
    }
  }
}

farcical::ui::MenuController::MenuController(Menu* menu, engine::EventSystem& eventSystem) : Entity(engine::EntityID{
    menu->GetID() + std::string{MenuControllerName}
  }),
  MouseInterface(),
  KeyboardInterface(),
  menu{menu}, eventSystem{eventSystem} {
}

void farcical::ui::MenuController::ReceiveMouseMovement(sf::Vector2i position) {
  const auto& focusList{menu->GetFocusList()};
  for(const auto& widget: focusList) {
    if(IsPointWithinRect(position, widget->GetBounds())) {
      widget->DoAction(Action{Action::Type::ReceiveFocus});
    } // if cursor position intersects this Widget's bounds
    else {
      widget->DoAction(Action{Action::Type::LoseFocus});
    } // else cursor position does not intersect this Widget's bounds
  } // for each focusable Widget in focusList
}

void farcical::ui::MenuController::ReceiveMouseButtonPress(sf::Mouse::Button buttonPressed, sf::Vector2i position) {
  const auto& focusList{menu->GetFocusList()};
  int index = 0;
  for(const auto& widget: focusList) {
    if(IsPointWithinRect(position, widget->GetBounds())) {
      menu->SetSelectedIndex(index);
      widget->DoAction(Action{Action::Type::SetPressedTrue});
    } // if cursor position intersects this Widget's bounds
    else if(menu->GetSelectedIndex() == index) {
      widget->DoAction(Action{Action::Type::LoseFocus});
    } // else if cursor does not intersect the bounds of currently selected Widget, that Widget loses focus
    ++index;
  } // for each focusable Widget in focusList
}

void farcical::ui::MenuController::ReceiveMouseButtonRelease(sf::Mouse::Button buttonReleased, sf::Vector2i position) {
  const auto& focusList{menu->GetFocusList()};
  int index = 0;
  for(const auto& widget: focusList) {
    if(widget->GetType() == Widget::Type::Button) {
      Button* button{dynamic_cast<Button*>(widget)};
      if(button->GetStatus() == Button::Status::Pressed) {
        button->DoAction(Action{Action::Type::SetPressedFalse});
        if(IsPointWithinRect(position, button->GetBounds())) {
          const engine::Event::Parameters& eventParams{button->GetOnPressEvent()};
          eventSystem.Enqueue(engine::Event{eventParams.type, eventParams.args});
        } // if cursor position intersects with this Button's bounds
      } // if buttonStatus == Pressed
    } // if Button

    else if(widget->GetType() == Widget::Type::RadioButton) {
      RadioButton* radioButton{dynamic_cast<RadioButton*>(widget)};
      if(IsPointWithinRect(position, radioButton->GetBounds())) {
        menu->SetSelectedIndex(index);
        radioButton->DoAction(Action{Action::Type::SetSelectedTrue});
      } // if cursor position intersects with this RadioButton's bounds
      else if(menu->GetSelectedIndex() != index) {
        radioButton->DoAction(Action{Action::Type::SetSelectedFalse});
      } // else if cursor does not intersect the bounds of currently selected RadioButton, that RadioButton is unselected
    } // else if RadioButton

    ++index;
  } // for each focusable Widget in focusList
}

void farcical::ui::MenuController::ReceiveKeyboardInput(sf::Keyboard::Key input) {
  bool selectionMoved{false};
  int selectedIndex{menu->GetSelectedIndex()};

  if(input == sf::Keyboard::Key::Up || input == sf::Keyboard::Key::Left) {
    menu->DoAction(Action{Action::Type::MoveSelectionUp});
    selectionMoved = true;
  } // if input == Up || Left
  else if(input == sf::Keyboard::Key::Down || input == sf::Keyboard::Key::Right) {
    menu->DoAction(Action{Action::Type::MoveSelectionDown});
    selectionMoved = true;
  } // else if input == Down || Right
  else if(input == sf::Keyboard::Key::Enter) {
    if(menu->GetMenuType() == Menu::Type::Button) {
      const auto& selectedItem{menu->GetSelectedItem()};
      if(!selectedItem.has_value()) {
        return;
      } // if no MenuItem is selected
      const auto& button{std::get_if<Button*>(&selectedItem.value())};
      const engine::Event::Parameters& onPressEvent{(*button)->GetOnPressEvent()};
      eventSystem.Enqueue(engine::Event{onPressEvent.type, onPressEvent.args});
    } // if Button
  } // else if input == Enter

  if(selectionMoved) {
    Widget* previouslySelected{menu->GetWidget(selectedIndex)};
    if(previouslySelected) {
      previouslySelected->DoAction(Action{Action::Type::LoseFocus});
    } // if previouslySelected
    selectedIndex = menu->GetSelectedIndex();
    Widget* currentlySelected{menu->GetWidget(selectedIndex)};
    if(currentlySelected) {
      currentlySelected->DoAction(Action{Action::Type::ReceiveFocus});
    } // if currentlySelected
  } // if selectionMoved
}
