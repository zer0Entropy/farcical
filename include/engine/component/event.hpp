//
// Created by dgmuller on 8/14/25.
//

#ifndef EVENT_COMPONENT_HPP
#define EVENT_COMPONENT_HPP

#include "component.hpp"
#include "../event.hpp"

namespace farcical::engine {
    struct EventComponent final : public Component {
        std::vector<Event::Type> handledTypes;
        EventHandler* handler;

        EventComponent() = delete;

        explicit EventComponent(EntityID parentID) : Component(Component::Type::Event, parentID),
                                                     handler{nullptr} {
        }

        ~EventComponent() override = default;

        [[nodiscard]] bool CanHandleEventType(Event::Type type) const {
            bool canHandle{false};
            for(const auto& handled: handledTypes) {
                if(handled == type) {
                    canHandle = true;
                    break;
                } // if matching EventType is found
            } // for each EventType in handledTypes
            return canHandle;
        }
    };
}

#endif //EVENT_COMPONENT_HPP