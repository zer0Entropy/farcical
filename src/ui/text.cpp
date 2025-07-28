//
// Created by dgmuller on 5/30/25.
//

#include <SFML/Graphics/Text.hpp>
#include "../../include/ui/text.hpp"

farcical::ui::Text::Text(engine::EntityID id, Container* parent):
  Widget(id, Widget::Type::Text, parent, false),
  font{nullptr},
  contents{""} {
}

void farcical::ui::Text::SetFont(sf::Font& font) {
  this->font = &font;
}

void farcical::ui::Text::SetFontSize(unsigned int fontSize) {
  fontProperties.characterSize = fontSize;
}

void farcical::ui::Text::SetFontColor(sf::Color color) {
  fontProperties.color = color;
}

void farcical::ui::Text::SetOutlineColor(sf::Color color) {
  fontProperties.outlineColor = color;
}

void farcical::ui::Text::SetOutlineThickness(float thickness) {
  fontProperties.outlineThickness = thickness;
}

void farcical::ui::Text::SetContents(std::string_view contents) {
  this->contents = contents;
  if(font) {
    sf::Text text{*font, this->contents, fontProperties.characterSize};
    this->size = sf::Vector2u{
      static_cast<unsigned int>(text.getLocalBounds().size.x),
      static_cast<unsigned int>(text.getLocalBounds().size.y)
    };
  } // if font
}

void farcical::ui::Text::SetFontProperties(const FontProperties& properties) {
  this->fontProperties = properties;
}

std::string_view farcical::ui::Text::GetContents() const {
  return contents;
}

sf::Font* farcical::ui::Text::GetFont() const {
  return font;
}

const farcical::FontProperties& farcical::ui::Text::GetFontProperties() const {
  return fontProperties;
}

void farcical::ui::Text::DoAction(Action action) {
}
