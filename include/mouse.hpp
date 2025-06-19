//
// Created by dgmuller on 6/18/25.
//

#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <SFML/Window/Mouse.hpp>

namespace farcical {
    class MouseInterface {
    public:
        explicit MouseInterface() = default;
        MouseInterface(const MouseInterface&) = delete;
        MouseInterface(MouseInterface&) = delete;
        MouseInterface(MouseInterface&&) = delete;
        virtual ~MouseInterface() = default;
        MouseInterface& operator=(const MouseInterface&) = delete;

        virtual void ReceiveMouseMovement(sf::Vector2i position) = 0;
        virtual void ReceiveMouseButtonClick(sf::Mouse::Button button, sf::Vector2i position) = 0;
    };
}

#endif //MOUSE_HPP
