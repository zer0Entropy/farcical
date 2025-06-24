//
// Created by dgmuller on 6/22/25.
//

#ifndef EVENT_HPP
#define EVENT_HPP

#include <deque>
#include "system.hpp"

namespace farcical {
    struct Event {
        enum class Type {
            QuitGame,
            TransitionMainMenu,
            TransitionNewGame,
            TransitionLoadGame,
            TransitionOptions
        };

        Type type;
    };

    class EventPropagator {
    public:
        explicit EventPropagator(EventPropagator* next): next{next} {
        }

        virtual ~EventPropagator() = default;

        virtual void ReceiveEvent(const Event& event) { next->ReceiveEvent(event); }

    protected:
        EventPropagator* next;
    };

    namespace game {
        class Game;
    }

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

    private:
        game::Game& game;
        Engine& engine;
        std::deque<Event> eventQueue;
    };
}

#endif //EVENT_HPP
