//
// Created by dgmuller on 6/7/25.
//

#ifndef GAME_HPP
#define GAME_HPP

#include "../engine/engine.hpp"
#include "../ui/scene.hpp"
#include "world.hpp"
#include "player.hpp"
#include "../ui/label.hpp"
#include "../ui/config.hpp"

namespace farcical::game {
    class Game {
    public:

        Game() = delete;
        Game(Game const&) = delete;
        Game(Game &&) = delete;
        Game& operator=(Game const&) = delete;

        explicit Game(engine::Engine& engine);

        ~Game() = default;

        std::optional<engine::Error> Init(const ResourceList& sceneResourceList);

        std::optional<engine::Error> Update();

        std::unique_ptr<World> CreateWorld();

        std::unique_ptr<Player> CreatePlayer();

        std::expected<std::unique_ptr<ui::Scene>, engine::Error> CreateScene(ui::SceneConfig config, ui::Scene* parent);

    private:
        std::optional<engine::Error> CreateSceneLayout(ui::Scene& scene, const ui::LayoutConfig& layoutConfig);

        std::optional<engine::Error> CreateDecorations(ui::Scene& scene,
                                                       const std::vector<ui::DecorationConfig>& decorationConfigList);

        std::optional<engine::Error> CreateTitle(ui::Scene& scene, const ui::LabelConfig& titleConfig);

        std::optional<engine::Error> CreateMenu(ui::Scene& scene, const ui::MenuConfig& menuConfig);

        std::optional<engine::Error> CacheFonts(ui::Scene& scene,
                                                const std::vector<FontProperties>& fontPropertiesList);

        std::optional<engine::Error> CacheTextures(ui::Scene& scene,
                                                   const std::vector<TextureProperties>& texturePropertiesList);

        std::optional<engine::Error> CacheRepeatingTextures(ui::Scene& scene,
                                                            const std::vector<RepeatingTextureProperties>&
                                                            texturePropertiesList);

        std::optional<engine::Error> CacheSegmentedTextures(ui::Scene& scene,
                                                            const std::vector<SegmentedTextureProperties>&
                                                            texturePropertiesList);

        engine::Engine& engine;
        ui::SceneHierarchy sceneHierarchy;
        std::unordered_map<engine::EntityID, ResourceParameters> sceneResources;

    };
}

#endif //GAME_HPP
