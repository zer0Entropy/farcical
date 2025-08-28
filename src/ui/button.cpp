//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/button.hpp"
#include "../../include/engine/system/render.hpp"

farcical::ui::Button::Button(engine::EntityID id, const engine::Event::Parameters& onPress, Container* parent)
    : Widget(id, Widget::Type::Button, parent, true),
      textures{nullptr},
      status{Status::Normal},
      onPressEvent{onPress} {
}

void farcical::ui::Button::SetStatus(Status status) {
    this->status = status;
    engine::RenderComponent* renderCmp{
        dynamic_cast<engine::RenderComponent*>(this->GetComponent(engine::Component::Type::Render))
    };
    renderCmp->texture = this->textures[static_cast<int>(status)];
}

void farcical::ui::Button::SetTexture(Status status, sf::Texture& texture) {
    this->textures[static_cast<int>(status)] = &texture;
    this->size = sf::Vector2u{
        texture.getSize().x * static_cast<unsigned int>(this->scale.x),
        texture.getSize().y * static_cast<unsigned int>(this->scale.y)
    };
}

farcical::ui::Button::Status farcical::ui::Button::GetStatus() const {
    return status;
}

sf::Texture* farcical::ui::Button::GetTexture(Status buttonStatus) const {
    return textures[static_cast<int>(buttonStatus)];
}

sf::Texture* farcical::ui::Button::GetTexture() const {
    return textures[static_cast<int>(status)];
}

const farcical::engine::Event::Parameters& farcical::ui::Button::GetOnPressEvent() const {
    return onPressEvent;
}

void farcical::ui::Button::DoAction(Action action) {
    if(action.type == Action::Type::ReceiveFocus) {
        //  Normal --> Highlighted
        if(status == Status::Normal) {
            SetStatus(Status::Highlighted);
        } // if buttonStatus == Normal
    } // if action == ReceiveFocus

    else if(action.type == Action::Type::LoseFocus) {
        //  Highlighted --> Normal
        // or Pressed --> Normal
        if(status == Status::Highlighted || status == Status::Pressed) {
            SetStatus(Status::Normal);
        } // if buttonStatus == Highlighted || buttonStatus == Pressed
    } // else if action == LoseFocus

    else if(action.type == Action::Type::SetPressedTrue) {
        SetStatus(Status::Pressed);
    } // else if action == SetPressedTrue

    else if(action.type == Action::Type::SetPressedFalse) {
        SetStatus(Status::Normal);
    } // else if action == SetPressedFalse
}