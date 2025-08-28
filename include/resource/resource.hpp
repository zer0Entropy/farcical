//
// Created by dgmuller on 6/4/25.
//

#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <string>
#include <SFML/Graphics/Rect.hpp>
#include "../engine/id.hpp"

namespace farcical {

    using ResourceID = std::string;

    using ResourceParameters = std::pair<ResourceID, std::string>;
    using ResourceList = std::vector<std::pair<engine::EntityID, ResourceParameters> >;

    struct ResourceHandle {
        enum class Status {
            Uninitialized,
            IsReady,
            Error
        };

        enum class Type {
            Unknown = -1,
            Log,
            JSONDocument,
            Font,
            Texture,
            Sound,
            Music,
            NumResourceTypes
        };

        ResourceID          id;
        Status              status;
        Type                type;
        std::string         path;

        constexpr static Type GetTypeFromName(std::string_view name) {
            Type type{Type::Unknown};
            if(name == "log") {
                type = Type::Log;
            } // log
            else if(name == "jsonDocument") {
                type = Type::JSONDocument;
            } // jsonDocument
            else if(name == "font") {
                type = Type::Font;
            } // font
            else if(name == "texture") {
                type = Type::Texture;
            } // texture
            else if(name == "music") {
                type = Type::Music;
            } // music
            else if(name == "sound") {
                type = Type::Sound;
            } // sound
            else {
                type = Type::Unknown;
            } // unknown
            return type;
        }

        constexpr static std::string_view GetTypeName(Type type) {
            std::string_view name{"?"};
            switch(type) {
                case Type::Log: {
                    name = "log";
                }
                break;
                case Type::JSONDocument: {
                    name = "jsonDocument";
                }
                break;
                case Type::Font: {
                    name = "font";
                }
                break;
                case Type::Texture: {
                    name = "texture";
                }
                break;
                case Type::Music: {
                    name = "music";
                }
                break;
                case Type::Sound: {
                    name = "sound";
                }
                break;
                case Type::Unknown: {
                    name = "?";
                }
            } // switch(type)
            return name;
        }
    };

}

#endif //RESOURCE_HPP
