//
// Created by dgmuller on 6/22/25.
//

#ifndef EVENT_SYSTEM_HPP
#define EVENT_SYSTEM_HPP

#include <array>
#include <expected>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include "../component/event.hpp"
#include "../entity.hpp"
#include "../error.hpp"
#include "system.hpp"

namespace farcical::game {
    class Game;
}

namespace farcical::engine {
    class Engine;

    class EventSystem final : public System {
    public:
        EventSystem() = delete;

        EventSystem(EventSystem&) = delete;

        EventSystem(const EventSystem&) = delete;

        explicit EventSystem(game::Game& game, Engine& engine);

        ~EventSystem() override = default;

        void Enqueue(const Event& event);

        void Init() override;

        void Update() override;

        void Stop() override;

        std::expected<EventComponent*, Error> CreateEventComponent(
            const std::vector<Event::Type>& handledTypes, EventHandler* handler, EntityID parentID);

        std::optional<Error> DestroyEventComponent(EntityID parentID);

    private:
        game::Game& game;
        Engine& engine;

        std::vector<Event> eventQueue;

        std::unordered_map<EntityID, std::unique_ptr<EventComponent> > components;

        using EventHandlerList = std::vector<EventHandler*>;
        std::array<EventHandlerList, static_cast<int>(Event::Type::NumEventTypes)> handlers;

        EventHandlerList& GetEventHandlerList(Event::Type type) const;
    };
}

#endif //EVENT_SYSTEM_HPP
