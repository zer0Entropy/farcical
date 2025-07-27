//
// Created by dgmuller on 7/6/25.
//

#ifndef UI_CONFIG_HPP
#define UI_CONFIG_HPP

#include <vector>
#include "../engine/error.hpp"
#include "../resource/config.hpp"
#include "../engine/event.hpp"
#include "layout.hpp"
#include "menu.hpp"

namespace farcical::ui {
    struct DecorationConfig {
        engine::EntityID id;
        engine::EntityID parentID;
        sf::Vector2u relativePosition;
        TextureProperties textureProperties;

        DecorationConfig(): id{""},
                            parentID{""},
                            relativePosition{sf::Vector2u{0, 0}} {
        }

        ~DecorationConfig() = default;
    };

    struct ButtonConfig {
        engine::EntityID id;
        std::vector<std::pair<Button::Status, TextureProperties> > textureProperties;

        ButtonConfig(): id{""} {
        }

        ~ButtonConfig() = default;
    };

    struct LabelConfig {
        engine::EntityID id;
        engine::EntityID parentID;
        sf::Vector2u relativePosition;
        std::string contents;
        FontProperties fontProperties;

        LabelConfig(): id{""},
                       parentID{""},
                       relativePosition{sf::Vector2u{0, 0}},
                       contents{""} {
        }

        ~LabelConfig() = default;
    };

    struct MenuItemConfig {
        engine::EntityID id;
        ButtonConfig buttonConfig;
        LabelConfig labelConfig;
        engine::Event::Type activationEventType;
        std::vector<std::any> activationEventArgs;

        MenuItemConfig(): id{""},
                          activationEventType{engine::Event::Type::QuitGame} {
        }

        ~MenuItemConfig() = default;
    };

    struct MenuConfig {
        engine::EntityID id;
        engine::EntityID parentID;
        MenuLayout layout;
        sf::Vector2u relativePosition;
        std::vector<MenuItemConfig> menuItemConfigs;
        ButtonConfig buttonConfig;
        FontProperties fontProperties;

        explicit MenuConfig(): id{""},
                               parentID{""},
                               relativePosition{sf::Vector2u{0, 0}} {
        }
        ~MenuConfig() = default;
    };

    struct LayoutLayerConfig {
        Layout::Layer::ID id;
        std::vector<DecorationConfig> decorations;
        LabelConfig title;
        std::vector<LabelConfig> headings;
        MenuConfig menu;

        LayoutLayerConfig(): id{Layout::Layer::ID::NumLayers} {
        }

        ~LayoutLayerConfig() = default;
    };

    struct LayoutConfig {
        std::array<LayoutLayerConfig, static_cast<int>(Layout::Layer::ID::NumLayers)> layers;

        LayoutConfig() {
            for(int index = 0; index < static_cast<int>(Layout::Layer::ID::NumLayers); ++index) {
                layers[index].id = static_cast<Layout::Layer::ID>(index);
            } // for each layer
        }

        ~LayoutConfig() = default;
    };

    struct SceneConfig {
        engine::EntityID id;
        std::vector<FontProperties> fonts;
        std::vector<TextureProperties> textures;
        std::vector<RepeatingTextureProperties> repeatingTextures;
        std::vector<SegmentedTextureProperties> segmentedTextures;
        BorderTextureProperties borderTexture;
        LayoutConfig layout;

        SceneConfig(): id{""} {
        }

        ~SceneConfig() = default;

        std::optional<FontProperties> FindFontProperties(ResourceID id) const {
            for(const auto& fontProperties: fonts) {
                if(fontProperties.id == id) {
                    return fontProperties;
                }
            } // for each fontProperties in fonts
            return std::nullopt;
        }

        std::optional<TextureProperties> FindTextureProperties(ResourceID id) const {
            for(const auto& textureProperties: textures) {
                if(textureProperties.id == id) {
                    return textureProperties;
                }
            } // for each textureProperties in textures
            return std::nullopt;
        }

        std::optional<RepeatingTextureProperties> FindRepeatingTextureProperties(ResourceID id) const {
            for(const auto& textureProperties: repeatingTextures) {
                if(textureProperties.outputID == id) {
                    return textureProperties;
                }
            } // for each repeatingTexture in repeatingTextures
            return std::nullopt;
        }

        std::optional<SegmentedTextureProperties> FindSegmentedTextureProperties(ResourceID id) const {
            for(const auto& textureProperties: segmentedTextures) {
                if(textureProperties.id == id) {
                    return textureProperties;
                }
            } // for each segmentedTexture in segmentedTextures
            return std::nullopt;
        }
    };

    std::expected<DecorationConfig, engine::Error> LoadDecorationConfig(const nlohmann::json& json);

    std::expected<ButtonConfig, engine::Error> LoadButtonConfig(const nlohmann::json& json);

    std::expected<LabelConfig, engine::Error> LoadLabelConfig(const nlohmann::json& json);

    std::expected<MenuItemConfig, engine::Error> LoadMenuItemConfig(const nlohmann::json& json);

    std::expected<MenuConfig, engine::Error> LoadMenuConfig(const nlohmann::json& json);

    std::expected<LayoutLayerConfig, engine::Error> LoadLayoutLayerConfig(const nlohmann::json& json);

    std::expected<LayoutConfig, engine::Error> LoadLayoutConfig(const nlohmann::json& json);

    std::expected<SceneConfig, engine::Error> LoadSceneConfig(const nlohmann::json& json);
}

#endif //UI_CONFIG_HPP
