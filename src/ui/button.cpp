//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/button.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include "../../include/ui/button.hpp"

farcical::ui::Button::Button(std::string_view name, Widget* parent)
  : Widget(name, Widget::Type::Button, parent, false),
    textures{nullptr},
    status{Status::Normal} {
}

void farcical::ui::Button::SetTexture(Status state, sf::Texture& texture) {
  this->textures[static_cast<int>(state)] = &texture;
}

sf::Vector2u farcical::ui::Button::GetSize() const {
  sf::Texture& texture = *this->textures[static_cast<int>(Status::Normal)];
  sf::Vector2u textureSize{texture.getSize()};
  return sf::Vector2u{textureSize.x * static_cast<unsigned int>(this->scale.x), textureSize.y * static_cast<unsigned int>(this->scale.y)};
}

farcical::ui::Button::Status farcical::ui::Button::GetStatus() const {
  return status;
}

void farcical::ui::Button::SetStatus(Status status) {
  this->status = status;
}

void farcical::ui::Button::Draw(sf::RenderTarget& target) const {
  sf::Texture& texture = *this->textures[static_cast<int>(this->status)];
  sf::Sprite sprite{texture};
  sprite.setPosition(position);
  sprite.setScale(scale);
  target.draw(sprite);
}

void farcical::ui::Button::DoAction(Action action) {
}
