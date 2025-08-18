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

farcical::ui::MenuController::MenuController(Menu* menu, engine::EventSystem& eventSystem) : Entity(engine::EntityID{
    menu->GetID() + std::string{MenuControllerName}
  }),
  MouseInterface(),
  KeyboardInterface(),
  menu{menu}, eventSystem{eventSystem} {
}

void farcical::ui::MenuController::ReceiveMouseMovement(sf::Vector2i position) {
  const auto& itemList{menu->GetItems()};
  for(const auto& item: itemList) {
    const auto& button{std::get_if<Button*>(&item)};
    const auto& radioButton{std::get_if<RadioButton*>(&item)};
    const auto& subMenu{std::get_if<Menu*>(&item)};
    Widget* widget{nullptr};
    if(button) {
      widget = *button;
    } // if Button
    else if(radioButton) {
      widget = *radioButton;
    } // if radioButton
    else if(subMenu) {
      widget = *subMenu;
    } // if subMenu
    else {
      return;
    }

    if(IsPointWithinRect(position, widget->GetBounds())) {
      widget->DoAction(Action{Action::Type::ReceiveFocus});
    } // if this Item is under the cursor
    else {
      // if(menu->GetSelectedItem() == item) {
      widget->DoAction(Action{Action::Type::LoseFocus});
    } // else if selectedItem == item
  } // for each Item in Menu
}

void farcical::ui::MenuController::ReceiveMouseButtonPress(sf::Mouse::Button buttonPressed, sf::Vector2i position) {
  const auto& itemList{menu->GetItems()};
  for(const auto& item: itemList) {
    const auto& button{std::get_if<Button*>(&item)};
    const auto& radioButton{std::get_if<RadioButton*>(&item)};
    const auto& subMenu{std::get_if<Menu*>(&item)};
    Widget* widget{nullptr};
    if(button) {
      widget = *button;
    } // if Button
    else if(radioButton) {
      widget = *radioButton;
    } // if radioButton
    else if(subMenu) {
      widget = *subMenu;
    } // if subMenu
    else {
      return;
    }

    if(IsPointWithinRect(position, widget->GetBounds())) {
      menu->SetSelectedItem(item);
      widget->DoAction(Action{Action::Type::SetPressedTrue});
    } // if this Item is under the cursor
    else if(menu->GetSelectedItem() == item) {
      widget->DoAction(Action{Action::Type::LoseFocus});
    } // else if selectedItem == item
  } // for each Item in Menu
}

void farcical::ui::MenuController::ReceiveMouseButtonRelease(sf::Mouse::Button buttonReleased, sf::Vector2i position) {
  const auto& itemList{menu->GetItems()};
  for(const auto& item: itemList) {
    const auto& button{std::get_if<Button*>(&item)};
    const auto& radioButton{std::get_if<RadioButton*>(&item)};
    const auto& subMenu{std::get_if<Menu*>(&item)};
    if(button) {
      if((*button)->GetStatus() == Button::Status::Pressed) {
        (*button)->DoAction(Action{Action::Type::SetPressedFalse});
        if(IsPointWithinRect(position, (*button)->GetBounds())) {
          const engine::Event::Parameters& eventParams{(*button)->GetOnPressEvent()};
          eventSystem.Enqueue(engine::Event{eventParams.type, eventParams.args});
        } // if cursor is within Button's bounds
      } // if Button status == Pressed
    } // if Button
    else if(radioButton) {
    } // if radioButton
    else if(subMenu) {
    } // if subMenu
    else {
      return;
    }
  } // for each Item in Menu
  menu->SetSelectedIndex(-1);
}

void farcical::ui::MenuController::ReceiveKeyboardInput(sf::Keyboard::Key input) {
  const auto& getSelected{menu->GetSelectedItem()};
  const Menu::Item* selectedItem{nullptr};
  if(getSelected.has_value()) {
    selectedItem = &getSelected.value();
  }
  const auto& itemList{menu->GetItems()};
  for(const auto& item: itemList) {
    const auto& button{std::get_if<Button*>(&item)};
    const auto& radioButton{std::get_if<RadioButton*>(&item)};
    const auto& subMenu{std::get_if<Menu*>(&item)};
    Widget* widget{nullptr};
    bool isSelected{false};
    if(button) {
      widget = *button;
      if(auto selectedButton = std::get_if<Button*>(selectedItem)) {
        isSelected = true;
      } // if this Button is selected
    } // if Button
    else if(radioButton) {
      widget = *radioButton;
      if(auto selectedRadio = std::get_if<RadioButton*>(selectedItem)) {
        isSelected = true;
      } // if this RadioButton is selected
    } // if radioButton
    else if(subMenu) {
      widget = *subMenu;
      if(auto selectedMenu = std::get_if<Menu*>(selectedItem)) {
        isSelected = true;
      } // if this Menu is selected
    } // if subMenu
    else {
      return;
    } // else widgetType is invalid

    if(selectedItem && isSelected) {
      widget->DoAction(Action{Action::Type::LoseFocus});
    } // if selectedItem

    if(input == sf::Keyboard::Key::Up || input == sf::Keyboard::Key::Left) {
      menu->DoAction(Action{Action::Type::MoveSelectionUp});
    } // if input == Up || Left
    else if(input == sf::Keyboard::Key::Down || input == sf::Keyboard::Key::Right) {
      menu->DoAction(Action{Action::Type::MoveSelectionDown});
    } // else if input == Down || Right
    else if(input == sf::Keyboard::Key::Enter) {
      if(button) {
        const engine::Event::Parameters& onPressEvent{(*button)->GetOnPressEvent()};
        eventSystem.Enqueue(engine::Event{onPressEvent.type, onPressEvent.args});
      } // if Button
      return;
    } // else if input == Enter

    Widget* selectedWidget{menu->GetWidget(menu->GetSelectedIndex())};
    if(selectedWidget) {
      selectedWidget->DoAction(Action{Action::Type::ReceiveFocus});
    } // if selectedWidget
  } // for each Item
}
