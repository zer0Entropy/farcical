//
// Created by dgmuller on 6/24/25.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "config.hpp"
#include "container.hpp"
#include "layout.hpp"
#include "menu.hpp"
#include "../engine/error.hpp"
#include "../resource/manager.hpp"

namespace farcical::ui {

    class Scene final : public Container {
    public:
        Scene() = delete;
        Scene(const Scene&) = delete;
        Scene(Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;

        explicit Scene(engine::EntityID id);

        ~Scene() override = default;

        void DoAction(Action action) override;

        [[nodiscard]] std::vector<sf::Text*> GetTextLayer(Layout::Layer::ID layerID) const;

        [[nodiscard]] std::vector<sf::Sprite*> GetSpriteLayer(Layout::Layer::ID layerID) const;

        [[nodiscard]] sf::Font* GetCachedFont(ResourceID id) const;

        [[nodiscard]] FontProperties GetCachedFontProperties(ResourceID id) const;

        [[nodiscard]] sf::Texture* GetCachedTexture(ResourceID id) const;

        [[nodiscard]] TextureProperties GetCachedTextureProperties(ResourceID id) const;

        void CacheFont(ResourceID id, sf::Font* font);

        void CacheFontProperties(ResourceID id, const FontProperties& fontProperties);

        void CacheTexture(ResourceID id, sf::Texture* texture);

        void CacheTextureProperties(ResourceID id, const TextureProperties& textureProperties);

    private:

        Widget* focusedWidget;

        std::unordered_map<ResourceID, sf::Font*> fontCache;
        std::unordered_map<ResourceID, FontProperties> fontPropertiesCache;

        std::unordered_map<ResourceID, sf::Texture*> textureCache;
        std::unordered_map<ResourceID, TextureProperties> texturePropertiesCache;

        std::array<std::unordered_map<std::string, sf::Text>, static_cast<int>(Layout::Layer::ID::NumLayers)>
        textLayers;
        std::array<std::unordered_map<std::string, sf::Sprite>, static_cast<int>(ui::Layout::Layer::ID::NumLayers)>
        spriteLayers;
    };

}

#endif //SCENE_HPP
