//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/menu.hpp"
#include "../../include/ui/button.hpp"
#include "../../include/ui/label.hpp"

int farcical::ui::MenuItem::fontSize = 24;
sf::Color farcical::ui::MenuItem::fontColor = sf::Color::White;
sf::Color farcical::ui::MenuItem::outlineColor = sf::Color::Red;

farcical::ui::MenuItem::MenuItem(std::string_view name, Widget* parent):
  Widget(name, Widget::Type::MenuItem, parent),
  button{nullptr},
  label{nullptr} {

}

farcical::ui::Button* farcical::ui::MenuItem::CreateButton(std::string_view name, sf::Texture& texture) {
  children.emplace_back(std::make_unique<Button>(name, this));
  button = dynamic_cast<Button*>(children.rbegin()->get());
  button->SetTexture(texture);
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

void farcical::ui::MenuItem::Draw(sf::RenderTarget& target) const {
  for(const auto& child : children) {
    child->Draw(target);
  }
}

farcical::ui::Menu::Menu(std::string_view name, Widget* parent):
  Widget(name, Widget::Type::Menu, parent),
  buttonTexture{nullptr},
  labelFont{nullptr},
  selectedIndex{-1} {

}

farcical::ui::MenuItem* farcical::ui::Menu::CreateMenuItem(std::string_view name, float buttonSpacing) {
  children.emplace_back(std::make_unique<MenuItem>(name, this));
  MenuItem* item{dynamic_cast<MenuItem*>(children.rbegin()->get())};
  std::string buttonName{std::string{name} + "Button"};
  std::string labelName{std::string{name}};

  Button* button{item->CreateButton(buttonName, *buttonTexture)};
  Label* label{item->CreateLabel(labelName, *labelFont)};

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
  for(const auto& child : children) {
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

void farcical::ui::Menu::SetButtonTexture(sf::Texture& texture) {
  buttonTexture = &texture;
}

void farcical::ui::Menu::SetLabelFont(sf::Font& font) {
  labelFont = &font;
}

void farcical::ui::Menu::SetSelectedIndex(int index) {
  selectedIndex = index;
}

void farcical::ui::Menu::Draw(sf::RenderTarget& target) const {
  for(const auto& child : children) {
    child->Draw(target);
  }
}
