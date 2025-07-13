//
// Created by dgmuller on 6/24/25.
//
#include <SFML/Graphics/Sprite.hpp>
#include "../../include/ui/decoration.hpp"

farcical::ui::Decoration::Decoration(engine::EntityID id, Widget* parent): Widget(id, Widget::Type::Decoration, parent,
                                                                               false),
                                                                           texture{nullptr} {
}

void farcical::ui::Decoration::DoAction(Action action) {
}

sf::Texture* farcical::ui::Decoration::GetTexture() const {
    return texture;
}

void farcical::ui::Decoration::SetTexture(sf::Texture* texture) {
    this->texture = texture;
}
