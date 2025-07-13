//
// Created by dgmuller on 7/12/25.
//

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "id.hpp"

namespace farcical::engine {
    struct Component {
        enum class Type {
            Render = 0,
            Input,
            Event,
            NumComponentTypes
        };

        Type type;
        EntityID parentID;

        Component() = delete;

        explicit Component(Type type, EntityID parentID): type{type}, parentID{parentID} {
        }

        virtual ~Component() = default;
    };
}

#endif //COMPONENT_HPP
