//
// Created by dgmuller on 6/22/25.
//

#ifndef EVENT_HPP
#define EVENT_HPP

#include <any>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "system.hpp"

namespace farcical::game {
    class Game;
}

namespace farcical::engine {

    struct Event {
        enum class Type {
            CreateScene,
            DestroyScene,
            QuitGame
        };

        Type type;
        std::vector<std::any> args;

        Event() = delete;
        explicit Event(Type type, const std::vector<std::any>& args = {}):
            type{type},
            args{args} {}
        virtual ~Event() = default;
    };

    class EventHandler {
    public:
        EventHandler() = default;
        virtual ~EventHandler() = default;
        virtual void HandleEvent(const Event& event) = 0;
    };

    class Engine;

    class EventSystem final : public System {
    public:
        EventSystem() = delete;
        EventSystem(EventSystem&) = delete;
        EventSystem(const EventSystem&) = delete;
        explicit EventSystem(game::Game& game, Engine& engine);
        ~EventSystem() override = default;

        void RegisterHandler(Event::Type type, EventHandler* handler);
        void UnregisterHandler(Event::Type type, EventHandler* handler);

        void Enqueue(const Event& event);

        void Init() override;

        void Update() override;

        void Stop() override;

    private:
        game::Game& game;
        Engine& engine;
        std::unordered_map<Event::Type, EventHandler*> handlers;
        std::deque<Event> eventQueue;
    };
}

#endif //EVENT_HPP
