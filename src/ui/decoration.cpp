//
// Created by dgmuller on 6/24/25.
//
#include <SFML/Graphics/Sprite.hpp>
#include "../../include/ui/decoration.hpp"

farcical::ui::Decoration::Decoration(std::string_view name, Widget* parent): Widget(name, Widget::Type::Decoration,
                                                                                 parent, false),
                                                                             texture{nullptr} {
}

void farcical::ui::Decoration::Draw(sf::RenderTarget& target) const {
    sf::Sprite sprite{*texture};
    sprite.setPosition(position);
    sprite.setScale(scale);
    target.draw(sprite);
}

void farcical::ui::Decoration::DoAction(Action action) {
}

void farcical::ui::Decoration::SetTexture(sf::Texture* texture) {
    this->texture = texture;
}
