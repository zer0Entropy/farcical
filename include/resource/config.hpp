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

    struct FontProperties {
        ResourceID id;
        std::string path;
        float scale;
        unsigned int characterSize;
        sf::Color color;
        sf::Color outlineColor;
        float outlineThickness;

        FontProperties():
            id{""},
            path{""},
            scale{1.0f},
            characterSize{0},
            color{sf::Color::Black},
            outlineColor{sf::Color::Black},
            outlineThickness(0.0f) {}

        FontProperties(
            ResourceID id,
            std::string_view path,
            float scale,
            unsigned int characterSize,
            sf::Color color,
            sf::Color outlineColor,
            float outlineThickness):
            id{id},
            path{path},
            scale{scale},
            characterSize{characterSize},
            color{color},
            outlineColor{outlineColor},
            outlineThickness{outlineThickness} {}

        ~FontProperties() = default;
    };

    struct TextureProperties {
        ResourceID id;
        std::string path;
        float scale;
        sf::IntRect inputRect;

        TextureProperties():
            id{""},
            path{""},
            scale{1.0f},
            inputRect{{0, 0}, {0, 0}} {}

        TextureProperties(ResourceID id, std::string_view path, float scale, const sf::IntRect& inputRect):
            id{id},
            path{path},
            scale{scale},
            inputRect{inputRect} {}

        ~TextureProperties() = default;
    };

    struct RepeatingTextureProperties {
        ResourceID inputID;
        ResourceID outputID;
        std::string path;
        float scale;
        sf::IntRect inputRect;
        sf::Vector2u outputSize;

        RepeatingTextureProperties():
            inputID{""},
            outputID{""},
            path{""},
            scale{1.0f},
            inputRect{{0, 0}, {0, 0}},
            outputSize{0, 0} {}

        RepeatingTextureProperties(
            ResourceID inputID,
            ResourceID outputID,
            std::string_view path,
            float scale,
            const sf::IntRect& inputRect,
            const sf::Vector2u& outputSize):
            inputID{inputID},
            outputID{outputID},
            path{path},
            scale{scale},
            inputRect{inputRect},
            outputSize{outputSize} {}

        ~RepeatingTextureProperties() = default;
    };

    struct SegmentedTextureProperties {
        ResourceID id;
        std::string path;
        float scale;
        sf::Vector2u outputSize;
        std::vector<TextureProperties> segments;

        SegmentedTextureProperties():
            id{""},
            path{""},
            scale{1.0f},
            outputSize{0, 0} {}

        SegmentedTextureProperties(ResourceID id, std::string_view path, float scale, const sf::Vector2u& outputSize):
            id{id},
            path{path},
            scale{scale},
            outputSize{outputSize} {}

        ~SegmentedTextureProperties() = default;
    };

    struct BorderTextureProperties {
        ResourceID id;
        std::string path;
        float scale;
        sf::Vector2u percentSize;
        sf::Vector2u outputSize;
        std::array<TextureProperties, static_cast<int>(Rectangle::Corner::NumCorners)> cornerTextures;
        std::array<TextureProperties, static_cast<int>(Rectangle::Edge::NumEdges)> edgeTextures;
        TextureProperties centerTexture;
    };

    std::expected<FontProperties, engine::Error>    LoadFontProperties(const nlohmann::json& json);
    std::expected<TextureProperties, engine::Error> LoadTextureProperties(const nlohmann::json& json, bool isSegment = false, std::string_view path = "");
    std::expected<RepeatingTextureProperties, engine::Error> LoadRepeatingTextureProperties(const nlohmann::json& json);
    std::expected<SegmentedTextureProperties, engine::Error> LoadSegmentedTextureProperties(const nlohmann::json& json);
    std::expected<BorderTextureProperties, engine::Error> LoadBorderTextureProperties(const nlohmann::json& json);
}

#endif //RESOURCE_CONFIG_HPP
