//
// Created by dgmuller on 6/18/25.
//

#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP
#include <SFML/Window/Keyboard.hpp>

namespace farcical {
    class KeyboardInterface {
    public:
        explicit KeyboardInterface() = default;
        KeyboardInterface(const KeyboardInterface&) = delete;
        KeyboardInterface(KeyboardInterface&) = delete;
        KeyboardInterface(KeyboardInterface&&) = delete;
        virtual ~KeyboardInterface() = default;
        KeyboardInterface& operator=(const KeyboardInterface&) = delete;

        virtual void ReceiveKeyboardInput(sf::Keyboard::Key input) = 0;
    };
}


#endif //KEYBOARD_HPP
