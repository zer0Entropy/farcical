//
// Created by dgmuller on 7/6/25.
//

#ifndef UI_CONFIG_HPP
#define UI_CONFIG_HPP

#include <vector>
#include "../engine/error.hpp"
#include "../resource/config.hpp"
#include "../engine/system/event.hpp"
#include "layout.hpp"
#include "menu.hpp"
#include "radio.hpp"

namespace farcical::ui {
    using TextProperties = std::pair<std::string, FontProperties>;

    struct WidgetProperties {
        engine::EntityID id;
        Widget::Type type;
        engine::EntityID parentID;
        Layout::Layer::ID layerID;
        sf::Vector2i relativePosition;
        TextProperties labelProperties;
    };

    struct DecorationProperties : public WidgetProperties {
        TextureProperties textureProperties;
    };

    struct ButtonProperties : public WidgetProperties {
        engine::Event::Parameters onPressEvent;
    };

    struct MenuProperties : public WidgetProperties {
        Menu::Type menuType;
        MenuLayout layout;
        std::vector<std::pair<Button::Status, TextureProperties> > buttonTextures;
        std::vector<std::pair<RadioButton::Status, TextureProperties> > radioButtonTextures;
        std::vector<ButtonProperties> buttonProperties;
        std::vector<WidgetProperties> radioButtonProperties;
        std::vector<MenuProperties> menuProperties;
    };

    struct LayoutLayerProperties {
        Layout::Layer::ID id;
        std::vector<DecorationProperties> decorationProperties;
        WidgetProperties titleProperties;
        std::vector<WidgetProperties> headingProperties;
        MenuProperties menuProperties;
    };

    struct LayoutProperties {
        std::array<LayoutLayerProperties, static_cast<int>(Layout::Layer::ID::NumLayers)> layers;

        LayoutProperties() {
            for(int index = 0; index < static_cast<int>(Layout::Layer::ID::NumLayers); ++index) {
                layers[index].id = static_cast<Layout::Layer::ID>(index);
            } // for each layer
        }

        ~LayoutProperties() = default;
    };

    struct SceneProperties {
        engine::EntityID id;
        std::vector<FontProperties> fonts;
        std::vector<TextureProperties> textures;
        std::vector<RepeatingTextureProperties> repeatingTextures;
        std::vector<SegmentedTextureProperties> segmentedTextures;
        BorderTextureProperties borderTexture;
        LayoutProperties layout;

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

    std::expected<WidgetProperties, engine::Error> LoadTextProperties(const nlohmann::json& json,
                                                                      engine::EntityID parentID);

    std::expected<DecorationProperties, engine::Error> LoadDecoration(const nlohmann::json& json,
                                                                      engine::EntityID parentID);

    std::expected<ButtonProperties, engine::Error> LoadButton(const nlohmann::json& json, engine::EntityID parentID);

    std::expected<WidgetProperties, engine::Error> LoadRadioButton(const nlohmann::json& json,
                                                                   engine::EntityID parentID);

    std::expected<MenuProperties, engine::Error> LoadMenu(const nlohmann::json& json, engine::EntityID parentID);

    std::expected<LayoutLayerProperties, engine::Error> LoadLayoutLayer(const nlohmann::json& json,
                                                                        engine::EntityID parentID);

    std::expected<LayoutProperties, engine::Error> LoadLayout(const nlohmann::json& json, engine::EntityID parentID);

    std::expected<SceneProperties, engine::Error> LoadScene(const nlohmann::json& json);

    std::expected<engine::EntityID, engine::Error> ExtractWidgetID(const nlohmann::json& json,
                                                                   std::string_view typeName);

    std::expected<Widget::Type, engine::Error> ExtractWidgetType(const nlohmann::json& json, engine::EntityID id);

    std::expected<Menu::Type, engine::Error> ExtractMenuType(const nlohmann::json& json, engine::EntityID id);

    std::expected<sf::Vector2i, engine::Error> ExtractRelativePosition(const nlohmann::json& json, engine::EntityID id);

    std::expected<Orientation, engine::Error> ExtractOrientation(const nlohmann::json& json, engine::EntityID id);

    std::expected<int, engine::Error> ExtractRelativeSpacing(const nlohmann::json& json, engine::EntityID id);

    std::expected<ResourceID, engine::Error> ExtractFontID(const nlohmann::json& json, engine::EntityID id);

    std::expected<ResourceID, engine::Error> ExtractTextureID(const nlohmann::json& json, engine::EntityID id);

    std::expected<std::string, engine::Error> ExtractTextContents(const nlohmann::json& json, engine::EntityID id);

    std::expected<engine::Event::Parameters, engine::Error> ExtractButtonEvent(
        const nlohmann::json& json, engine::EntityID id);

    std::expected<std::vector<ButtonProperties>, engine::Error> ExtractButtonProperties(
        const nlohmann::json& json, const MenuProperties& menuProperties);

    std::expected<std::vector<WidgetProperties>, engine::Error> ExtractRadioButtonProperties(
        const nlohmann::json& json, engine::EntityID id);

    std::expected<std::vector<std::pair<Button::Status, TextureProperties> >, engine::Error> ExtractButtonTextures(
        const nlohmann::json& json, engine::EntityID id);

    std::expected<std::vector<std::pair<RadioButton::Status, TextureProperties> >, engine::Error> ExtractRadioButtonTextures(
        const nlohmann::json& json, engine::EntityID id);
    std::expected<Layout::Layer::ID, engine::Error> ExtractLayerID(const nlohmann::json& json, engine::EntityID id);
}

#endif //UI_CONFIG_HPP
