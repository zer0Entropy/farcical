//
// Created by dgmuller on 7/27/25.
//
#include "../../include/ui/radio.hpp"
#include "../../include/engine/component/render.hpp"

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
    engine::RenderComponent* renderCmp{
        dynamic_cast<engine::RenderComponent*>(this->GetComponent(engine::Component::Type::Render))
    };
    renderCmp->texture = this->textures[static_cast<int>(status)];
}

void farcical::ui::RadioButton::DoAction(Action action) {
    if(action.type == Action::Type::SetSelectedTrue) {
        SetStatus(Status::Selected);
    } // if action == SetSelectedTrue

    else if(action.type == Action::Type::SetSelectedFalse) {
        SetStatus(Status::Unselected);
    } // else if action == SetSelectedFalse
}
