//
// Created by dgmuller on 6/24/25.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include <nlohmann/json.hpp>

#include "container.hpp"
#include "../resource/manager.hpp"
#include "../engine/event.hpp"

namespace farcical::ui {
    class Scene final : public Container, public engine::EventHandler {
    public:
        Scene() = delete;
        Scene(const Scene&) = delete;
        Scene(Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;

        explicit Scene(engine::EntityID id);

        ~Scene() override = default;

        [[nodiscard]] Widget* GetFocusedWidget() const;

        void SetFocusedWidget(Widget* widget);

        void DoAction(Action action) override;

        void HandleEvent(const engine::Event& event) override;

        [[nodiscard]] sf::Music* GetCachedMusic(ResourceID id) const;

        [[nodiscard]] MusicProperties GetCachedMusicProperties(ResourceID id) const;

        [[nodiscard]] sf::Font* GetCachedFont(ResourceID id) const;

        [[nodiscard]] FontProperties GetCachedFontProperties(ResourceID id) const;

        [[nodiscard]] sf::Texture* GetCachedTexture(ResourceID id) const;

        [[nodiscard]] TextureProperties GetCachedTextureProperties(ResourceID id) const;

        void CacheMusic(ResourceID id, sf::Music* music);

        void CacheMusicProperties(ResourceID id, const MusicProperties& musicProperties);

        void CacheFont(ResourceID id, sf::Font* font);

        void CacheFontProperties(ResourceID id, const FontProperties& fontProperties);

        void CacheTexture(ResourceID id, sf::Texture* texture);

        void CacheTextureProperties(ResourceID id, const TextureProperties& textureProperties);

        void ClearMusicCache();

        void ClearMusicPropertiesCache();

        void ClearFontCache();

        void ClearFontPropertiesCache();

        void ClearTextureCache();

        void ClearTexturePropertiesCache();

        /*
        [[nodiscard]] std::expected<MenuController*, engine::Error> CreateMenuController(
            Menu* menu, engine::EventSystem& eventSystem);

        std::optional<engine::Error> DestroyMenuController(Menu* menu, engine::EventSystem& eventSystem);

        [[nodiscard]] MenuController* GetMenuController(engine::EntityID menuID);
        */

    private:
        Widget* focusedWidget;

        std::unordered_map<ResourceID, sf::Music*> musicCache;
        std::unordered_map<ResourceID, MusicProperties> musicPropertiesCache;

        std::unordered_map<ResourceID, sf::Font*> fontCache;
        std::unordered_map<ResourceID, FontProperties> fontPropertiesCache;

        std::unordered_map<ResourceID, sf::Texture*> textureCache;
        std::unordered_map<ResourceID, TextureProperties> texturePropertiesCache;

        //std::unordered_map<engine::EntityID, std::unique_ptr<MenuController> > menuControllers;
    };

}

#endif //SCENE_HPP
