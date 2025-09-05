//
// Created by dgmuller on 7/27/25.
//
#include "../../include/ui/radio.hpp"
#include "../../include/engine/component/render.hpp"

farcical::ui::RadioButton::Controller::Controller(RadioButton* radioButton, engine::EventSystem& eventSystem):
    KeyboardInterface(),
    MouseInterface(),
    radioButton{radioButton},
    eventSystem{eventSystem} {
}

void farcical::ui::RadioButton::Controller::ReceiveMouseMovement(sf::Vector2i position) {
    const auto& bounds{radioButton->GetBounds()};
    if(IsPointWithinRect(position, bounds)) {
        eventSystem.Enqueue(engine::Event{engine::Event::Type::SetFocus, std::vector<std::any>{radioButton->GetID()}});
        radioButton->DoAction(Action{Action::Type::ReceiveFocus});
    } // if cursor position is within bounds
    else {
        eventSystem.Enqueue(engine::Event{engine::Event::Type::SetFocus});
        radioButton->DoAction(Action{Action::Type::LoseFocus});
    } // else if Button is not normal
}

void farcical::ui::RadioButton::Controller::ReceiveMouseButtonPress(sf::Mouse::Button mouseButton,
                                                                    sf::Vector2i position) {
}

void farcical::ui::RadioButton::Controller::ReceiveMouseButtonRelease(sf::Mouse::Button mouseButton,
                                                                      sf::Vector2i position) {
    const auto& bounds{radioButton->GetBounds()};
    if(mouseButton == sf::Mouse::Button::Left) {
        if(IsPointWithinRect(position, bounds)) {
            radioButton->DoAction(Action{Action::Type::SetPressedTrue});
        } // if cursor position is within bounds
        else {
            radioButton->DoAction(Action{Action::Type::SetPressedFalse});
        } // else
    } // if Left-Click
}

void farcical::ui::RadioButton::Controller::ReceiveKeyboardInput(sf::Keyboard::Key input) {
}

farcical::ui::RadioButton::RadioButton(engine::EntityID id, engine::EventSystem& eventSystem, Container* parent):
    Widget(id, Type::RadioButton, parent, true),
    Focusable(),
    textures{nullptr},
    status{Status::Off},
    controller{std::make_unique<RadioButton::Controller>(this, eventSystem)} {
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
    if(action.type == Action::Type::SetPressedTrue) {
        SetStatus(Status::On);
    } // if action == SetSelectedTrue

    else if(action.type == Action::Type::SetPressedFalse) {
        SetStatus(Status::Off);
    } // else if action == SetSelectedFalse

    else if(action.type == Action::Type::ReceiveFocus) {
        this->OnReceiveFocus();
        focused = true;
    } // else if action == ReceiveFocus

    else if(action.type == Action::Type::LoseFocus) {
        this->OnLoseFocus();
        focused = false;
    } // else if action == LoseFocus
}

void farcical::ui::RadioButton::OnReceiveFocus() {
    // If this RadioButton already has focus, then there is nothing to do.
    if(focused) {
        return;
    } // if focused
    if(status == Status::Off) {
        SetStatus(RadioButton::Status::OffHasFocus);
    } // if status == Off
    else if(status == Status::On) {
        SetStatus(RadioButton::Status::OnHasFocus);
    } // else if status == On
}

void farcical::ui::RadioButton::OnLoseFocus() {
    // If this RadioButton doesn't have focus, then there is nothing to do.
    if(!focused) {
        return;
    } // if !focused
    if(status == Status::OffHasFocus) {
        SetStatus(RadioButton::Status::Off);
    } // if status == OffHasFocus
    else if(status == Status::OnHasFocus) {
        SetStatus(RadioButton::Status::On);
    } // else if status == OnHasFocus
}

farcical::ui::RadioButton::Controller* farcical::ui::RadioButton::GetController() const {
    return controller.get();
}
