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
#include "../engine/logInterface.hpp"

namespace farcical::game {
    class Game;

    class GameController final : public engine::EventHandler, public engine::LogInterface {
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

        std::optional<engine::Error> Init(const ResourceList& sceneResourceList);

        std::optional<engine::Error> Update();

        std::optional<engine::Error> Stop();

        std::unique_ptr<World> CreateWorld();

        std::unique_ptr<Player> CreatePlayer();

        std::expected<ui::Scene*, engine::Error> CreateScene(ui::SceneConfig config);

        std::optional<engine::Error> StartCurrentScene(ui::SceneConfig& config);

        std::optional<engine::Error> StopCurrentScene();

        ui::Scene* GetCurrentScene() const;

        std::expected<ResourceParameters, engine::Error> FindSceneResource(engine::EntityID sceneID) const;

    private:
        std::optional<engine::Error> CreateSceneLayout(const ui::LayoutConfig& layoutConfig);

        std::optional<engine::Error> DestroySceneLayout(const ui::LayoutConfig& layoutConfig);

        std::optional<engine::Error> CreateDecorations(const std::vector<ui::DecorationConfig>& decorationConfigList);

        std::optional<engine::Error> DestroyDecorations(const std::vector<ui::DecorationConfig>& decorationConfigList);

        std::optional<engine::Error> CreateTitle(const ui::LabelConfig& titleConfig);

        std::optional<engine::Error> DestroyTitle(const ui::LabelConfig& titleConfig);

        std::optional<engine::Error> CreateMenu(const ui::MenuConfig& menuConfig);

        std::optional<engine::Error> DestroyMenu(const ui::MenuConfig& menuConfig);

        std::optional<engine::Error> CacheFonts(const std::vector<FontProperties>& fontPropertiesList);

        std::optional<engine::Error> CacheTextures(const std::vector<TextureProperties>& texturePropertiesList);

        std::optional<engine::Error> CacheRepeatingTextures(const std::vector<RepeatingTextureProperties>& texturePropertiesList);

        std::optional<engine::Error> CacheSegmentedTextures(const std::vector<SegmentedTextureProperties>& texturePropertiesList);

        std::optional<engine::Error> CacheBorderTexture(const BorderTextureProperties& borderProperties);

        Status status;
        engine::Engine& engine;
        std::unique_ptr<GameController> controller;
        std::unique_ptr<ui::Scene> currentScene;
        std::unique_ptr<ui::MenuController> menuController;
        std::unordered_map<engine::EntityID, ui::SceneConfig> sceneConfigs;
        std::unordered_map<engine::EntityID, ResourceParameters> sceneResources;
    };
}

#endif //GAME_HPP
