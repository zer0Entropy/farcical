//
// Created by dgmuller on 6/24/25.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include <nlohmann/json.hpp>

#include "container.hpp"
#include "../resource/manager.hpp"

namespace farcical::ui {
    class Scene final {
    public:
        Scene() = delete;
        Scene(const Scene&) = delete;
        Scene(Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;
        explicit Scene(engine::EntityID id);

        ~Scene() = default;

        [[nodiscard]] engine::EntityID GetID() const;

        [[nodiscard]] ui::RootContainer& GetWidgetContainer() const;

        [[nodiscard]] std::vector<ui::Widget*> GetTopLevelWidgets() const;

        [[nodiscard]] ui::Widget* FindWidget(engine::EntityID widgetID) const;

        void AddWidget(std::unique_ptr<ui::Widget> widget, ui::Widget* parent = nullptr);

        void RemoveWidget(engine::EntityID widgetID, ui::Widget* parent = nullptr);

        [[nodiscard]] sf::Font* GetCachedFont(ResourceID id) const;

        [[nodiscard]] FontProperties GetCachedFontProperties(ResourceID id) const;

        [[nodiscard]] sf::Texture* GetCachedTexture(ResourceID id) const;

        [[nodiscard]] TextureProperties GetCachedTextureProperties(ResourceID id) const;

        void CacheFont(ResourceID id, sf::Font* font);

        void CacheFontProperties(ResourceID id, const FontProperties& fontProperties);

        void CacheTexture(ResourceID id, sf::Texture* texture);

        void CacheTextureProperties(ResourceID id, const TextureProperties& textureProperties);

    private:
        engine::EntityID id;
        std::unique_ptr<RootContainer> widgetContainer;

        std::unordered_map<ResourceID, sf::Font*> fontCache;
        std::unordered_map<ResourceID, FontProperties> fontPropertiesCache;

        std::unordered_map<ResourceID, sf::Texture*> textureCache;
        std::unordered_map<ResourceID, TextureProperties> texturePropertiesCache;

        static constexpr std::string_view rootContainerID = "rootContainer";
    };

}

#endif //SCENE_HPP
