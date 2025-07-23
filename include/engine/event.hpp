//
// Created by dgmuller on 6/22/25.
//

#ifndef EVENT_HPP
#define EVENT_HPP

#include <any>
#include <deque>
#include <unordered_map>
#include <vector>
#include "system.hpp"

namespace farcical::game {
    class Game;
}

namespace farcical::engine {
    struct Event {
        enum class Type {
            Unknown = -1,
            ApplyEngineConfig,
            CreateScene,
            DestroyScene,
            QuitGame
        };

        Type type;
        std::vector<std::any> args;

        Event() = delete;

        explicit Event(Type type, const std::vector<std::any>& args = {}): type{type} {
            // For some reason, copying arg directly results in an extra layer of indirection :(
            for(const auto& arg: args) {
                this->args.emplace_back(arg);
            }
        }

        virtual ~Event() = default;

        constexpr static std::string_view GetTypeName(Type type) {
            std::string_view name{"?"};
            switch(type) {
                case Type::Unknown: {
                    name = "?";
                } break;
                case Type::ApplyEngineConfig: {
                    name = "ApplyEngineConfig";
                } break;
                case Type::CreateScene: {
                    name = "CreateScene";
                } break;
                case Type::DestroyScene: {
                    name = "DestroyScene";
                } break;
                case Type::QuitGame: {
                    name = "QuitGame";
                } break;
            } // switch(type)
            return name;
        }

        constexpr static Type GetTypeByName(std::string_view name) {
            Type type{Type::Unknown};
            if(name == "ApplyEngineConfig") {
                type = Type::ApplyEngineConfig;
            } // ApplyEngineConfig
            else if(name == "CreateScene") {
                type = Type::CreateScene;
            } // CreateScene
            else if(name == "DestroyScene") {
                type = Type::DestroyScene;
            } // DestroyScene
            else if(name == "QuitGame") {
                type = Type::QuitGame;
            }
            else {
                type = Type::Unknown;
            } // Unknown
            return type;
        }
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
