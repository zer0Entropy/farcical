//
// Created by dgmuller on 5/30/25.
//

#include "../../include/ui/button.hpp"
#include "../../include/engine/system/render.hpp"

farcical::ui::Button::Controller::Controller(Button* button, engine::EventSystem& eventSystem) : KeyboardInterface(),
    MouseInterface(),
    button{button},
    eventSystem{eventSystem} {
}

void farcical::ui::Button::Controller::ReceiveMouseMovement(sf::Vector2i position) {
    if(button->GetStatus() == Button::Status::Disabled) {
        return;
    } // if disabled
    const auto& bounds{button->GetBounds()};
    if(IsPointWithinRect(position, bounds)) {
        button->DoAction(Action{Action::Type::ReceiveFocus});
    } // if cursor position is within bounds
    else if(button->GetStatus() != Button::Status::Normal) {
        button->DoAction(Action{Action::Type::LoseFocus});
    } // else if Button is not normal
}

void farcical::ui::Button::Controller::ReceiveMouseButtonPress(sf::Mouse::Button mouseButton, sf::Vector2i position) {
    if(button->GetStatus() == Button::Status::Disabled) {
        return;
    } // if disabled
    const auto& bounds{button->GetBounds()};
    if(IsPointWithinRect(position, bounds)) {
        button->DoAction(Action{Action::Type::SetPressedTrue});
    } // if cursor position is within bounds
    else if(button->GetStatus() != Button::Status::Normal) {
        button->DoAction(Action{Action::Type::SetPressedFalse});
        button->DoAction(Action{Action::Type::LoseFocus});
    } // else if Button is not normal
}

void farcical::ui::Button::Controller::ReceiveMouseButtonRelease(sf::Mouse::Button mouseButton, sf::Vector2i position) {
    if(button->GetStatus() == Button::Status::Disabled) {
        return;
    } // if disabled
    if(button->GetStatus() == Button::Status::Pressed) {
        button->DoAction(Action{Action::Type::SetPressedFalse});
    } // if pressed
    const auto& bounds{button->GetBounds()};
    if(IsPointWithinRect(position, bounds)) {
        const engine::Event::Parameters& eventParams{button->GetOnPressEvent()};
        eventSystem.Enqueue(engine::Event{eventParams.type, eventParams.args});
    } // if cursor position is within bounds
}

void farcical::ui::Button::Controller::ReceiveKeyboardInput(sf::Keyboard::Key input) {
    if(button->GetStatus() == Button::Status::Disabled) {
        return;
    } // if disabled
    if(input == sf::Keyboard::Key::Enter) {
        const engine::Event::Parameters& eventParams{button->GetOnPressEvent()};
        eventSystem.Enqueue(engine::Event{eventParams.type, eventParams.args});
    } // if Enter
}

farcical::ui::Button::Button(
    engine::EntityID id, const engine::Event::Parameters& onPress, engine::EventSystem& eventSystem,
    Container* parent) : Widget(id, Widget::Type::Button, parent, true),
                         textures{nullptr},
                         status{Status::Normal},
                         onPressEvent{onPress},
                         controller{std::make_unique<Button::Controller>(this, eventSystem)} {
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

farcical::ui::Button::Controller* farcical::ui::Button::GetController() const {
    return controller.get();
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
