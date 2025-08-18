//
// Created by dgmuller on 7/27/25.
//
#include "../../include/ui/radio.hpp"

farcical::ui::RadioButton::RadioButton(engine::EntityID id, Container* parent) : Widget(id, Type::RadioButton, parent),
    textures{nullptr},
    status{Status::Unselected} {
}

void farcical::ui::RadioButton::SetTexture(Status state, sf::Texture& texture) {
    textures[static_cast<int>(state)] = &texture;
}

sf::Texture* farcical::ui::RadioButton::GetTexture(Status state) const {
    return textures[static_cast<int>(state)];
}

sf::Texture* farcical::ui::RadioButton::GetTexture() const {
    return textures[static_cast<int>(status)];
}

farcical::ui::RadioButton::Status farcical::ui::RadioButton::GetStatus() const {
    return status;
}

void farcical::ui::RadioButton::SetStatus(Status status) {
    this->status = status;
}

void farcical::ui::RadioButton::DoAction(Action action) {
}