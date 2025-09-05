//
// Created by dgmuller on 8/14/25.
//

#ifndef EVENT_HPP
#define EVENT_HPP

#include <any>
#include <string>
#include <vector>

namespace farcical::engine {
    struct Event final {
        enum class Type {
            Unknown = -1,
            ApplyEngineConfig = 0,
            CreateScene,
            SetFocus,
            QuitGame,
            NumEventTypes
        };

        struct Parameters {
            Type type;
            std::vector<std::any> args;
        };

        Type type;
        std::vector<std::any> args;

        Event() = delete;

        explicit Event(Type type, const std::vector<std::any>& args = {}) : type{type} {
            // For some reason, copying arg directly results in an extra layer of indirection :(
            for(const auto& arg: args) {
                this->args.emplace_back(arg);
            }
        }

        explicit Event(const Parameters& param) : type{param.type} {
            // For some reason, copying arg directly results in an extra layer of indirection :(
            for(const auto& arg: param.args) {
                this->args.emplace_back(arg);
            }
        }

        virtual ~Event() = default;

        constexpr static std::string_view GetTypeName(Type type) {
            std::string_view name{"?"};
            switch(type) {
                case Type::Unknown: {
                    name = "?";
                }
                break;
                case Type::ApplyEngineConfig: {
                    name = "ApplyEngineConfig";
                }
                break;
                case Type::CreateScene: {
                    name = "CreateScene";
                }
                break;
                case Type::QuitGame: {
                    name = "QuitGame";
                }
                break;
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
            else if(name == "QuitGame") {
                type = Type::QuitGame;
            } // QuitGame
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
}

#endif //EVENT_HPP