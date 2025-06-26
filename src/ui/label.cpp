//
// Created by dgmuller on 5/30/25.
//

#include <SFML/Graphics/Text.hpp>
#include "../../include/ui/label.hpp"

farcical::ui::Label::Label(std::string_view name, Widget* parent): Widget(name, Widget::Type::Label, parent, false),
                                                                   font{nullptr},
                                                                   properties{
                                                                     .contents = "",
                                                                     .fontID = "",
                                                                     .fontSize = 0,
                                                                     .fontColor = sf::Color::White,
                                                                     .outlineColor = sf::Color::Black,
                                                                     .outlineThickness = 0.0f
                                                                   } {
}

void farcical::ui::Label::SetFont(sf::Font& font) {
  this->font = &font;
}

void farcical::ui::Label::SetFontSize(unsigned int fontSize) {
  properties.fontSize = fontSize;
}

void farcical::ui::Label::SetFontColor(sf::Color color) {
  properties.fontColor = color;
}

void farcical::ui::Label::SetOutlineColor(sf::Color color) {
  properties.outlineColor = color;
}

void farcical::ui::Label::SetOutlineThickness(float thickness) {
  properties.outlineThickness = thickness;
}

void farcical::ui::Label::SetContents(std::string_view contents) {
  properties.contents = contents;
}

const farcical::ui::TextProperties& farcical::ui::Label::GetTextProperties() const {
  return properties;
}

sf::Vector2u farcical::ui::Label::GetSize() const {
  sf::Text text{*font, properties.contents, properties.fontSize};
  return sf::Vector2u{
    static_cast<unsigned int>(text.getLocalBounds().size.x),
    static_cast<unsigned int>(text.getLocalBounds().size.y)
  };
}

void farcical::ui::Label::Draw(sf::RenderTarget& target) const {
  sf::Text text{*font, properties.contents, properties.fontSize};
  text.setFillColor(properties.fontColor);
  text.setOutlineColor(properties.outlineColor);
  text.setOutlineThickness(properties.outlineThickness);
  text.setCharacterSize(properties.fontSize);
  text.setPosition(position);
  target.draw(text);
}

void farcical::ui::Label::DoAction(Action action) {
}
