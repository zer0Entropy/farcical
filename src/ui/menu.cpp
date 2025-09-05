//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/menu.hpp"
#include "../../include/color.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/text.hpp"
#include "../../include/engine/system/render.hpp"
#include "../../include/game/game.hpp"

farcical::ui::Menu::Menu(engine::EntityID id,
                         Menu::Type type,
                         const MenuLayout& layout,
                         Container* parent) : Container(id, Widget::Type::Menu, parent, false),
                                              menuType{type},
                                              layout{layout},
                                              labelFont{nullptr},
                                              titleFont{nullptr} {
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

void farcical::ui::Menu::DoAction(Action action) {
}

/*
void farcical::ui::Menu::DoAction(Action action) {
  const auto& focusList{GetFocusList()};
  if(action.type == Action::Type::MoveFocusUp) {
    if(!focusedWidget) {
      focusedWidget = *(focusList.end() - 1);
      focusedWidget->DoAction(Action{Action::Type::ReceiveFocus});
      return;
    }
    focusedWidget->DoAction(Action{Action::Type::LoseFocus});

    for(auto focusIter = focusList.begin(); focusIter != focusList.end(); ++focusIter) {
      if(*focusIter == focusedWidget) {
        if(focusIter == focusList.begin()) {
          focusedWidget = *(focusList.end() - 1);
        } // if the first Widget is focused
        else {
          focusedWidget = *(focusIter - 1);
        } // else not first Widget focused
        focusedWidget->DoAction(Action{Action::Type::ReceiveFocus});
        break;
      } // if focusIter == focusedWidget
    } // for each focusable Widget in focusList
  } // if action.type == MoveFocusUp

  else if(action.type == Action::Type::MoveFocusDown) {
    if(!focusedWidget) {
      focusedWidget = *focusList.begin();
      focusedWidget->DoAction(Action{Action::Type::ReceiveFocus});
      return;
    }
    focusedWidget->DoAction(Action{Action::Type::LoseFocus});

    for(auto focusIter = focusList.begin(); focusIter != focusList.end(); ++focusIter) {
      if(*focusIter == focusedWidget) {
        if(focusIter == focusList.end() - 1) {
          focusedWidget = *focusList.begin();
        } // if the first Widget is focused
        else {
          focusedWidget = *(focusIter + 1);
        } // else not first Widget focused
        focusedWidget->DoAction(Action{Action::Type::ReceiveFocus});
        break;
      } // if focusIter == focusedWidget
    } // for each focusable Widget in focusList
  } // else if action.type == MoveFocusDown
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
  for(const auto& widget: focusList) {
    if(IsPointWithinRect(position, widget->GetBounds())) {
      menu->SetFocusedWidget(widget);
      widget->DoAction(Action{Action::Type::SetPressedTrue});
    } // if cursor position intersects this Widget's bounds
    else if(menu->GetFocusedWidget() == widget) {
      widget->DoAction(Action{Action::Type::LoseFocus});
    } // else if cursor does not intersect the bounds of currently selected Widget, that Widget loses focus
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
        menu->SetFocusedWidget(radioButton);
        radioButton->DoAction(Action{Action::Type::SetSelectedTrue});
      } // if cursor position intersects with this RadioButton's bounds
      else if(menu->GetFocusedWidget() != radioButton) {
        radioButton->DoAction(Action{Action::Type::SetSelectedFalse});
      } // else if cursor does not intersect the bounds of currently selected RadioButton, that RadioButton is unselected
    } // else if RadioButton

    ++index;
  } // for each focusable Widget in focusList
}

void farcical::ui::MenuController::ReceiveKeyboardInput(sf::Keyboard::Key input) {
  if(input == sf::Keyboard::Key::Up) {
    menu->DoAction(Action{Action::Type::MoveFocusUp});
  } // if Up
  else if(input == sf::Keyboard::Key::Down) {
    menu->DoAction(Action{Action::Type::MoveFocusDown});
  } // if Down
  else if(input == sf::Keyboard::Key::Left) {
    menu->DoAction(Action{Action::Type::MoveFocusLeft});
  } // if Left
  else if(input == sf::Keyboard::Key::Right) {
    menu->DoAction(Action{Action::Type::MoveFocusRight});
  } // if Right
  else if(input == sf::Keyboard::Key::Enter) {
    if(menu->GetMenuType() == Menu::Type::Button) {
      const auto& focusedWidget{menu->GetFocusedWidget()};
      if(!focusedWidget) {
        return;
      } // if no Widget is focused
      if(focusedWidget->GetType() == Widget::Type::Button) {
        Button* button{dynamic_cast<Button*>(focusedWidget)};
        const engine::Event::Parameters& onPressEvent{button->GetOnPressEvent()};
        eventSystem.Enqueue(engine::Event{onPressEvent.type, onPressEvent.args});
      } // if focusedWidget->type == Button
    } // if Button
  } // else if input == Enter
}
*/
