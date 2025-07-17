//
// Created by dgmuller on 6/7/25.
//

#ifndef GAME_HPP
#define GAME_HPP

#include "../engine/engine.hpp"
#include "../ui/scene.hpp"
#include "world.hpp"
#include "player.hpp"
#include "../ui/config.hpp"

namespace farcical::game {
    class Game;

    class GameController final : public engine::EventHandler {
    public:
        GameController() = delete;

        explicit GameController(Game& game);

        ~GameController() override = default;

        void HandleEvent(const engine::Event& event) override;

    private:
        Game& game;
    };

    class Game {
    public:
        enum class Status {
            Uninitialized = -2,
            Error = -1,
            StoppedSuccessfully = 0,
            IsRunning = 1
        };

        Game() = delete;
        Game(Game const&) = delete;
        Game(Game &&) = delete;
        Game& operator=(Game const&) = delete;
        explicit Game(engine::Engine& engine);
        ~Game() = default;

        Status GetStatus() const;

        engine::Engine& GetEngine() const;

        ui::SceneHierarchy& GetSceneHierarchy() const;

        std::optional<engine::Error> Init(const ResourceList& sceneResourceList);

        std::optional<engine::Error> Update();

        std::optional<engine::Error> Stop();

        std::unique_ptr<World> CreateWorld();

        std::unique_ptr<Player> CreatePlayer();

        std::expected<std::unique_ptr<ui::Scene>, engine::Error> CreateScene(ui::SceneConfig config, ui::Scene* parent);

        std::expected<ResourceParameters, engine::Error> FindSceneResource(engine::EntityID sceneID) const;

    private:
        std::optional<engine::Error> CreateSceneLayout(ui::Scene& scene, const ui::LayoutConfig& layoutConfig);

        std::optional<engine::Error> CreateDecorations(
            ui::Scene& scene,
            const std::vector<ui::DecorationConfig>& decorationConfigList);

        std::optional<engine::Error> CreateTitle(ui::Scene& scene, const ui::LabelConfig& titleConfig);

        std::optional<engine::Error> CreateMenu(ui::Scene& scene, const ui::MenuConfig& menuConfig);

        std::optional<engine::Error> CacheFonts(
            ui::Scene& scene,
            const std::vector<FontProperties>& fontPropertiesList);

        std::optional<engine::Error> CacheTextures(
            ui::Scene& scene,
            const std::vector<TextureProperties>& texturePropertiesList);

        std::optional<engine::Error> CacheRepeatingTextures(
            ui::Scene& scene,
            const std::vector<RepeatingTextureProperties>& texturePropertiesList);

        std::optional<engine::Error> CacheSegmentedTextures(
            ui::Scene& scene,
            const std::vector<SegmentedTextureProperties>& texturePropertiesList);

        std::optional<engine::Error> CacheBorderTexture(
            ui::Scene& scene,
            const BorderTextureProperties& borderProperties);

        Status status;
        engine::Engine& engine;
        GameController controller;
        ui::SceneHierarchy sceneHierarchy;
        std::unordered_map<engine::EntityID, ResourceParameters> sceneResources;
    };
}

#endif //GAME_HPP
