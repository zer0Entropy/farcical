//
// Created by dgmuller on 7/13/25.
//

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <array>
#include "component.hpp"

namespace farcical::engine {
    using ComponentList = std::array<Component*, static_cast<int>(Component::Type::NumComponentTypes)>;

    class Entity {
    public:
        Entity() = delete;

        explicit Entity(EntityID id): id{id},
                                      components{nullptr} {
        }

        virtual ~Entity() = default;

        [[nodiscard]] EntityID GetID() const { return id; }

        [[nodiscard]] const ComponentList& GetComponents() const { return components; }
        [[nodiscard]] Component* GetComponent(Component::Type type) const { return components[static_cast<int>(type)]; }

        [[nodiscard]] bool HasComponent(Component::Type type) const {
            return components[static_cast<int>(type)] != nullptr;
        }

        void AddComponent(Component* component) {
            components[static_cast<int>(component->type)] = component;
        }

        void RemoveComponent(Component::Type type) {
            components[static_cast<int>(type)] = nullptr;
        }

    protected:
        EntityID id;
        ComponentList components;
    };
}

#endif //ENTITY_HPP
