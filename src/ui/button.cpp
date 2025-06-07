//
// Created by dgmuller on 5/30/25.
//

#include <SFML/Graphics/Sprite.hpp>
#include "../../include/ui/button.hpp"

farcical::ui::Button::Button(std::string_view name, Widget* parent)
    : Widget(name, Widget::Type::Button, parent),
    texture{nullptr} {}

void farcical::ui::Button::SetTexture(sf::Texture& texture) {
  this->texture = &texture;
}

sf::Vector2u farcical::ui::Button::GetSize() const {
  sf::Vector2u textureSize{this->texture->getSize()};
  return sf::Vector2u{textureSize.x * static_cast<unsigned int>(this->scale.x), textureSize.y * static_cast<unsigned int>(this->scale.y)};
}

void farcical::ui::Button::Draw(sf::RenderTarget& target) const {
  sf::Sprite sprite{*texture};
  sprite.setPosition(position);
  sprite.setScale(scale);
  target.draw(sprite);
}
