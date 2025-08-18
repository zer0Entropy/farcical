//
// Created by dgmuller on 8/11/25.
//

#ifndef INPUT_COMPONENT_HPP
#define INPUT_COMPONENT_HPP

#include "component.hpp"
#include "../keyboard.hpp"
#include "../mouse.hpp"

namespace farcical::engine {
    struct InputComponent final : public Component {
        KeyboardInterface* keyboard;
        MouseInterface* mouse;

        InputComponent() = delete;

        explicit InputComponent(EntityID parentID) : Component(Type::Input, parentID),
                                                     keyboard{nullptr},
                                                     mouse{nullptr} {
        }

        ~InputComponent() override = default;
    };
}
#endif //INPUT_COMPONENT_HPP