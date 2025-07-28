//
// Created by dgmuller on 7/27/25.
//
#include "../../include/ui/radio.hpp"

farcical::ui::RadioButton::RadioButton(engine::EntityID id, Container* parent):
    Widget(id, Type::RadioButton, parent),
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

farcical::ui::RadioButtonGroup::RadioButtonGroup(engine::EntityID id, Container* parent):
    Container(id, Type::RadioButtonGroup, parent),
    selectedButton{nullptr} {
}

void farcical::ui::RadioButtonGroup::AddChild(std::unique_ptr<Widget> child) {
    const unsigned int childIndex{GetNumChildren()};
    Container::AddChild(std::move(child));
    Widget* movedChild(GetChild(childIndex));
    if(movedChild->GetType() == Type::RadioButton) {
        radioButtons.push_back(dynamic_cast<RadioButton*>(movedChild));
    } // if child is the appropriate type
}

farcical::ui::RadioButton* farcical::ui::RadioButtonGroup::GetSelectedButton() const {
    return selectedButton;
}

farcical::ui::RadioButton* farcical::ui::RadioButtonGroup::GetButtonByID(engine::EntityID id) const {
    Widget* widget{FindChild(id)};
    if(widget && widget->GetType() == Type::RadioButton) {
        return dynamic_cast<RadioButton*>(widget);
    } // if widget found
    return nullptr;
}

farcical::ui::RadioButton* farcical::ui::RadioButtonGroup::GetButtonByIndex(int index) const {
    RadioButton* button{nullptr};
    if(index < radioButtons.size()) {
        button = radioButtons[index];
    } // if index is valid
    return button;
}

void farcical::ui::RadioButtonGroup::SetSelectedButton(RadioButton* button) {
    selectedButton = button;
}

void farcical::ui::RadioButtonGroup::DoAction(Action action) {

}
