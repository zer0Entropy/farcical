//
// Created by dgmuller on 5/30/25.
//

#include <SFML/Graphics/Text.hpp>
#include "../../include/ui/label.hpp"

farcical::ui::Label::Label(engine::EntityID id, Widget* parent): Widget(id, Widget::Type::Label, parent, false),
                                                                 font{nullptr},
                                                                 contents{""} {
}

void farcical::ui::Label::SetFont(sf::Font& font) {
  this->font = &font;
}

void farcical::ui::Label::SetFontSize(unsigned int fontSize) {
  fontProperties.characterSize = fontSize;
}

void farcical::ui::Label::SetFontColor(sf::Color color) {
  fontProperties.color = color;
}

void farcical::ui::Label::SetOutlineColor(sf::Color color) {
  fontProperties.outlineColor = color;
}

void farcical::ui::Label::SetOutlineThickness(float thickness) {
  fontProperties.outlineThickness = thickness;
}

void farcical::ui::Label::SetContents(std::string_view contents) {
  this->contents = contents;
}

void farcical::ui::Label::SetFontProperties(const FontProperties& properties) {
  this->fontProperties = properties;
}

std::string_view farcical::ui::Label::GetContents() const {
  return contents;
}

sf::Font* farcical::ui::Label::GetFont() const {
  return font;
}

const farcical::FontProperties& farcical::ui::Label::GetFontProperties() const {
  return fontProperties;
}

sf::Vector2u farcical::ui::Label::GetSize() const {
  sf::Text text{*font, contents, fontProperties.characterSize};
  return sf::Vector2u{
    static_cast<unsigned int>(text.getLocalBounds().size.x),
    static_cast<unsigned int>(text.getLocalBounds().size.y)
  };
}

void farcical::ui::Label::DoAction(Action action) {
}
