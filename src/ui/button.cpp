//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/button.hpp"
#include "../../include/engine/render.hpp"

farcical::ui::Button::Button(engine::EntityID id, Widget* parent)
  : Widget(id, Widget::Type::Button, parent, false),
    textures{nullptr},
    status{Status::Normal} {
}

void farcical::ui::Button::SetTexture(Status state, sf::Texture& texture) {
  this->textures[static_cast<int>(state)] = &texture;
}

sf::Texture* farcical::ui::Button::GetTexture(Status buttonStatus) const {
  return textures[static_cast<int>(buttonStatus)];
}

sf::Texture* farcical::ui::Button::GetTexture() const {
  return textures[static_cast<int>(status)];
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
  engine::RenderComponent* renderCmp{
    dynamic_cast<engine::RenderComponent*>(this->GetComponent(engine::Component::Type::Render))
  };
  renderCmp->texture = this->textures[static_cast<int>(status)];
}

void farcical::ui::Button::DoAction(Action action) {
}
