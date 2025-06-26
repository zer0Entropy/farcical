//
// Created by dgmuller on 6/9/25.
//
#include "../include/input.hpp"

farcical::InputSystem::InputSystem(sf::RenderWindow& window, ui::Manager& manager): System(System::ID::InputSystem),
    window{window}, uiManager{manager} {
}

void farcical::InputSystem::Init() {
}

void farcical::InputSystem::Update() {
    if(window.isOpen()) {
        while(const std::optional event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
                break;
            } // if(event == Closed)

            else if(const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                for(const auto& listener: mouseListeners) {
                    listener->ReceiveMouseMovement(mouseMoved->position);
                }
            } // else if(event == MouseMoved)

            else if(const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                for(const auto& listener: mouseListeners) {
                    listener->ReceiveMouseButtonPress(mouseButtonPressed->button, mouseButtonPressed->position);
                } // for each mouseListener
            } // else if(event == MouseButtonPressed)

            else if(const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                for(const auto& listener: mouseListeners) {
                    listener->ReceiveMouseButtonRelease(mouseButtonReleased->button, mouseButtonReleased->position);
                } // for each mouseListener
            } // else if(event == MouseButtonReleased)

            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                for(const auto& listener: keyListeners) {
                    listener->ReceiveKeyboardInput(keyPressed->code);
                } // for each keyListener
            } // else if(event == KeyPressed)
        } // while(event = pollEvent())
    } // if(window isOpen)
}

void farcical::InputSystem::Stop() {
}

void farcical::InputSystem::AddMouseListener(MouseInterface* listener) {
    mouseListeners.push_back(listener);
}

void farcical::InputSystem::AddKeyListener(KeyboardInterface* listener) {
    keyListeners.push_back(listener);
}

void farcical::InputSystem::RemoveMouseListener(MouseInterface* listener) {
    for(auto listenIter = mouseListeners.begin(); listenIter != mouseListeners.end(); ++listenIter) {
        if(*listenIter == listener) {
            mouseListeners.erase(listenIter);
            return;
        }
    }
}

void farcical::InputSystem::RemoveKeyListener(KeyboardInterface* listener) {
    for(auto listenIter = keyListeners.begin(); listenIter != keyListeners.end(); ++listenIter) {
        if(*listenIter == listener) {
            keyListeners.erase(listenIter);
            return;
        }
    }
}
