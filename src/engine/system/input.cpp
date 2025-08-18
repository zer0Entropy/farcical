//
// Created by dgmuller on 6/9/25.
//
#include "../../../include/engine/system/input.hpp"

farcical::engine::InputSystem::InputSystem(sf::RenderWindow& window,
                                           LogSystem& logSystem) : System(System::ID::InputSystem, logSystem),
                                                                   window{window} {
}

void farcical::engine::InputSystem::Init() {
    WriteToLog("InputSystem initialized.");
}

void farcical::engine::InputSystem::Update() {
    if(window.isOpen()) {
        while(const std::optional event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
                break;
            } // if(event == Closed)

            // Broadcast Mouse Movements
            if(const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                for(const auto& listener: mouseListeners) {
                    listener->ReceiveMouseMovement(mouseMoved->position);
                }
            } // else if(event == MouseMoved)

            // Broadcast Mouse Button Presses
            else if(const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                for(const auto& listener: mouseListeners) {
                    listener->ReceiveMouseButtonPress(mouseButtonPressed->button, mouseButtonPressed->position);
                } // for each mouseListener
            } // else if(event == MouseButtonPressed)

            // Broadcast Mouse Button Releases
            else if(const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                for(const auto& listener: mouseListeners) {
                    listener->ReceiveMouseButtonRelease(mouseButtonReleased->button, mouseButtonReleased->position);
                } // for each mouseListener
            } // else if(event == MouseButtonReleased)

            // Broadcast Key Presses
            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                for(const auto& listener: keyboardListeners) {
                    listener->ReceiveKeyboardInput(keyPressed->code);
                } // for each keyListener
            } // else if(event == KeyPressed)
        } // while(event = pollEvent())
    } // if(window isOpen)
}

void farcical::engine::InputSystem::Stop() {
    WriteToLog("InputSystem successfully shut down.");
}

std::expected<farcical::engine::InputComponent*, farcical::engine::Error>
farcical::engine::InputSystem::CreateInputComponent(
    MouseInterface* mouse, KeyboardInterface* keyboard, EntityID parentID) {
    const auto& createComponentResult{
        components.emplace(parentID, std::make_unique<InputComponent>(parentID))
    };
    if(!createComponentResult.second) {
        const std::string failMsg{"Failed to create InputComponent for Widget with ID=\"" + parentID + "\"."};
        return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
    } // if createComponentResult == failure
    InputComponent* component{createComponentResult.first->second.get()};
    component->keyboard = keyboard;
    component->mouse = mouse;
    if(keyboard) {
        keyboardListeners.emplace_back(component->keyboard);
    } // if keyboard
    if(mouse) {
        mouseListeners.emplace_back(component->mouse);
    } // if mouse
    return component;
}

std::optional<farcical::engine::Error> farcical::engine::InputSystem::DestroyInputComponent(EntityID parentID) {
    const auto& findComponent{components.find(parentID)};
    if(findComponent == components.end()) {
        const std::string failMsg{
            "Error: Could not find InputComponent for Widget with ID=\"" + parentID +
            "\", which was requested to be destroyed."
        };
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    } // if findComponent == failure
    InputComponent* component{findComponent->second.get()};
    if(component->keyboard) {
        for(auto keyIter = keyboardListeners.begin(); keyIter != keyboardListeners.end(); ++keyIter) {
            if(*keyIter == component->keyboard) {
                keyboardListeners.erase(keyIter);
                break;
            } // if keyboardListener for parentID is found
        } // for each keyboardListener
    } // if keyboard
    if(component->mouse) {
        for(auto mouseIter = mouseListeners.begin(); mouseIter != mouseListeners.end(); ++mouseIter) {
            if(*mouseIter == component->mouse) {
                mouseListeners.erase(mouseIter);
                break;
            } // if mouseListener for parentID is found
        } // for each mouseListener
    } // if mouse
    components.erase(findComponent);
    return std::nullopt;
}
