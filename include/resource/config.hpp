//
// Created by dgmuller on 7/6/25.
//

#ifndef RESOURCE_CONFIG_HPP
#define RESOURCE_CONFIG_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/Color.hpp>
#include "resource.hpp"
#include "../engine/error.hpp"
#include "../geometry.hpp"

namespace farcical {
    struct ResourceProperties {
        /* ResourceID id:  Unique identifier used by ResourceManager to store/retrieve Resources (along with Type)
         * Resource::Type type: Enum value (one of: Unknown, Log, JSONDocument, Font, Texture, Sound, Music)
         * std::string path: Location on disk from whence this Resource will be loaded
         * bool persist: Flag used to denote that this Resource should not be destroyed when changing Scenes
        */
        ResourceID id;
        ResourceHandle::Type type;
        std::string path;
        bool persist;

        ResourceProperties() : id{""},
                               type{ResourceHandle::Type::Unknown},
                               path{""},
                               persist{false} {
        }

        explicit ResourceProperties(ResourceHandle::Type type) : id{""},
                                                                 type{type},
                                                                 path{""},
                                                                 persist{false} {
        }

        explicit ResourceProperties(ResourceID id, ResourceHandle::Type type, std::string_view path,
                                    bool persist = false) : id{id},
                                                            type{type},
                                                            path{path},
                                                            persist{persist} {
        }

        ~ResourceProperties() = default;
    };

    struct MusicProperties : public ResourceProperties {
        bool loop;
        float volume;

        MusicProperties() : ResourceProperties(ResourceHandle::Type::Music),
                            loop{true},
                            volume{100.0} {
        }

        MusicProperties(ResourceID id, std::string_view path, bool loop, float volume,
                        bool persist = false) : ResourceProperties(id, ResourceHandle::Type::Music, path, persist),
                                                loop{loop},
                                                volume{volume} {
        }

        ~MusicProperties() = default;
    };

    struct FontProperties : public ResourceProperties {
        float scale;
        unsigned int characterSize;
        sf::Color color;
        sf::Color outlineColor;
        float outlineThickness;

        FontProperties() : ResourceProperties(ResourceHandle::Type::Font),
                           scale{1.0f},
                           characterSize{0},
                           color{sf::Color::Black},
                           outlineColor{sf::Color::Black},
                           outlineThickness(0.0f) {}

        FontProperties( ResourceID id,
                        std::string_view path,
                        float scale,
                        unsigned int characterSize,
                        sf::Color color,
                        sf::Color outlineColor,
                        float outlineThickness,
                        bool persist = false):
            ResourceProperties(id, ResourceHandle::Type::Font, path, persist),
            scale{scale},
            characterSize{characterSize},
            color{color},
            outlineColor{outlineColor},
            outlineThickness{outlineThickness} {
        }

        ~FontProperties() = default;
    };

    struct TextureProperties : public ResourceProperties {
        float scale;
        sf::Color color;
        bool isColorized;
        sf::IntRect inputRect;

        TextureProperties():
            ResourceProperties(ResourceHandle::Type::Texture),
            scale{1.0f},
            color{sf::Color::Black},
            isColorized{false},
            inputRect{{0, 0}, {0, 0}} {
        }

        TextureProperties(ResourceID id,
                          std::string_view path,
                          float scale,
                          const sf::IntRect& inputRect,
                          bool persist = false) : ResourceProperties(id, ResourceHandle::Type::Texture, path, persist),
                                                  scale{scale},
                                                  color{sf::Color::Black},
                                                  isColorized{false},
                                                  inputRect{inputRect} {
        }

        TextureProperties(  ResourceID id,
                            std::string_view path,
                            float scale,
                            const sf::Color& color,
                            const sf::IntRect& inputRect,
                            bool persist = false) : ResourceProperties(id, ResourceHandle::Type::Texture, path,
                                                                       persist),
                                                    scale{scale},
                                                    color{color},
                                                    isColorized{true},
                                                    inputRect{inputRect} {
        }

        ~TextureProperties() = default;
    };

    struct RepeatingTextureProperties : public ResourceProperties {
        ResourceID inputID;
        ResourceID outputID;
        float scale;
        sf::Color color;
        bool isColorized;
        sf::IntRect inputRect;
        sf::Vector2u outputSize;

        RepeatingTextureProperties() : ResourceProperties(ResourceHandle::Type::Texture),
                                       inputID{""},
                                       outputID{""},
                                       scale{1.0f},
                                       color{sf::Color::Black},
                                       isColorized{false},
                                       inputRect{{0, 0}, {0, 0}},
                                       outputSize{0, 0} {
        }

        RepeatingTextureProperties(ResourceID inputID,
                                   ResourceID outputID,
                                   std::string_view path,
                                   float scale,
                                   const sf::IntRect& inputRect,
                                   const sf::Vector2u& outputSize,
                                   bool persist = false) : ResourceProperties(outputID, ResourceHandle::Type::Texture,
                                                                              path, persist),
                                                           inputID{inputID},
                                                           outputID{outputID},
                                                           scale{scale},
                                                           color{sf::Color::Black},
                                                           isColorized{false},
                                                           inputRect{inputRect},
                                                           outputSize{outputSize} {
        }

        RepeatingTextureProperties(ResourceID inputID,
                                   ResourceID outputID,
                                   std::string_view path,
                                   float scale,
                                   const sf::Color& color,
                                   const sf::IntRect& inputRect,
                                   const sf::Vector2u& outputSize,
                                   bool persist = false) : ResourceProperties(outputID, ResourceHandle::Type::Texture,
                                                                              path, persist),
                                                           inputID{inputID},
                                                           outputID{outputID},
                                                           scale{scale},
                                                           color{color},
                                                           isColorized{true},
                                                           inputRect{inputRect},
                                                           outputSize{outputSize} {

        }

        ~RepeatingTextureProperties() = default;
    };

    struct SegmentedTextureProperties : public ResourceProperties {
        float scale;
        sf::Color color;
        bool isColorized;
        sf::Vector2u outputSize;
        std::vector<TextureProperties> segments;

        SegmentedTextureProperties() : ResourceProperties(ResourceHandle::Type::Texture),
                                       scale{1.0f},
                                       color{sf::Color::Black},
                                       isColorized{false},
                                       outputSize{0, 0} {
        }

        SegmentedTextureProperties(ResourceID id,
                                   std::string_view path,
                                   float scale,
                                   const sf::Vector2u& outputSize,
                                   bool persist = false) : ResourceProperties(id, ResourceHandle::Type::Texture, path,
                                                                              persist),
                                                           scale{scale},
                                                           color{sf::Color::Black},
                                                           isColorized{false},
                                                           outputSize{outputSize} {
        }

        SegmentedTextureProperties(ResourceID id,
                                   std::string_view path,
                                   float scale,
                                   const sf::Color& color,
                                   const sf::Vector2u& outputSize,
                                   bool persist = false) : ResourceProperties(id, ResourceHandle::Type::Texture, path, persist),
                                                           scale{scale},
                                                           color{color},
                                                           isColorized{true},
                                                           outputSize{outputSize} {}

        ~SegmentedTextureProperties() = default;
    };

    struct BorderTextureProperties: public ResourceProperties {
        float scale;
        sf::Vector2u percentSize;
        std::array<TextureProperties, static_cast<int>(Rectangle::Corner::NumCorners)> cornerTextures;
        std::array<TextureProperties, static_cast<int>(Rectangle::Edge::NumEdges)> edgeTextures;
        TextureProperties centerTexture;

        BorderTextureProperties():
            ResourceProperties(ResourceHandle::Type::Texture),
            scale{1.0f},
            percentSize{100, 100} {
        }

        BorderTextureProperties(    ResourceID id,
                                    std::string_view path,
                                    float scale,
                                    const sf::Vector2u& percentSize,
                                    const std::vector<TextureProperties>& cornerTextures,
                                    const std::vector<TextureProperties>& edgeTextures,
                                    const TextureProperties& centerTexture,
                                    bool persist = false):
            ResourceProperties(id, ResourceHandle::Type::Texture, path, persist),
            scale{scale},
            percentSize{percentSize},
            centerTexture{centerTexture} {
            int cornerIndex = 0;
            for(const auto& corner: cornerTextures) {
                this->cornerTextures[cornerIndex++] = corner;
            }
            int edgeIndex = 0;
            for(const auto& edge: edgeTextures) {
                this->edgeTextures[edgeIndex++] = edge;
            }
        }

        ~BorderTextureProperties() = default;
    };

    std::expected<MusicProperties, engine::Error>   LoadMusicProperties(const nlohmann::json& json);
    std::expected<FontProperties, engine::Error>    LoadFontProperties(const nlohmann::json& json);
    std::expected<TextureProperties, engine::Error> LoadTextureProperties(const nlohmann::json& json, bool isSegment = false, std::string_view path = "");
    std::expected<RepeatingTextureProperties, engine::Error> LoadRepeatingTextureProperties(const nlohmann::json& json);
    std::expected<SegmentedTextureProperties, engine::Error> LoadSegmentedTextureProperties(const nlohmann::json& json);

    std::expected<BorderTextureProperties, engine::Error> LoadBorderTextureProperties(const nlohmann::json& json);
}

#endif //RESOURCE_CONFIG_HPP
