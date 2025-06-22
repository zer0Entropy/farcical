//
// Created by dgmuller on 5/30/25.
//

#include <SFML/Graphics/Text.hpp>
#include "../../include/ui/label.hpp"

farcical::ui::Label::Label(std::string_view name, Widget* parent):
  Widget(name, Widget::Type::Label, parent),
  font{nullptr},
  fontSize{0},
  outlineThickness{0.1f},
  contents{name},
  fontColor{sf::Color::White},
  outlineColor{sf::Color::Black} {

}

void farcical::ui::Label::SetFont(sf::Font& font) {
  this->font = &font;
}

void farcical::ui::Label::SetFontSize(unsigned int fontSize) {
  this->fontSize = fontSize;
}

void farcical::ui::Label::SetFontColor(sf::Color color) {
  this->fontColor = color;
}

void farcical::ui::Label::SetOutlineColor(sf::Color color) {
  this->outlineColor = color;
}

void farcical::ui::Label::SetOutlineThickness(float thickness) {
  this->outlineThickness = thickness;
}

void farcical::ui::Label::SetContents(std::string_view contents) {
  this->contents = contents;
}

sf::Vector2u farcical::ui::Label::GetSize() const {
  sf::Text text{*font, contents, fontSize};
  return sf::Vector2u{
    static_cast<unsigned int>(text.getLocalBounds().size.x),
    static_cast<unsigned int>(text.getLocalBounds().size.y)
  };
}

void farcical::ui::Label::Draw(sf::RenderTarget& target) const {
  sf::Text text{*font, contents, fontSize};
  text.setFillColor(fontColor);
  text.setOutlineColor(outlineColor);
  text.setOutlineThickness(outlineThickness);
  text.setCharacterSize(fontSize);
  text.setPosition(position);
  target.draw(text);
}

void farcical::ui::Label::DoAction(Action action) {

}
