//
// Created by dgmuller on 8/17/25.
//
#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include <memory>
#include "config.hpp"
#include "scene.hpp"
#include "../engine/logInterface.hpp"

namespace farcical::engine {
    class Engine;
}

namespace farcical::ui {
    class SceneManager final : public engine::LogInterface {
    public:
        SceneManager() = delete;

        explicit SceneManager(engine::Engine& engine);

        ~SceneManager() override = default;

        SceneManager(const SceneManager&) = delete;

        SceneManager(SceneManager&&) = delete;

        SceneManager& operator=(const SceneManager&) = delete;

        SceneManager& operator=(SceneManager&&) = delete;

        std::optional<engine::Error> LoadResourceIndex(std::string_view indexPath);

        [[nodiscard]] std::expected<SceneProperties, engine::Error> GetCurrentSceneProperties() const;

        [[nodiscard]] std::expected<SceneProperties, engine::Error> FindSceneProperties(engine::EntityID id) const;

        [[nodiscard]] Scene* GetCurrentScene() const;

        [[nodiscard]] std::expected<Scene*, engine::Error> SetCurrentScene(engine::EntityID id);

        std::optional<engine::Error> DestroyCurrentScene();

        static constexpr std::string_view MainMenuSceneID{"mainMenuScene"};

    private:
        std::optional<engine::Error> BuildPropertiesCache();

        std::optional<engine::Error> BuildResourceCache(const SceneProperties& properties) const;

        std::optional<engine::Error> DestroyResourceCache(const SceneProperties& properties) const;

        std::optional<engine::Error> BuildFontCache(const std::vector<FontProperties>& fonts) const;

        std::optional<engine::Error> DestroyFontCache(const std::vector<FontProperties>& fonts) const;

        std::optional<engine::Error> BuildTextureCache(const std::vector<TextureProperties>& textures) const;

        std::optional<engine::Error> DestroyTextureCache(const std::vector<TextureProperties>& textures) const;

        std::optional<engine::Error> BuildRepeatingTextureCache(
            const std::vector<RepeatingTextureProperties>& textures) const;

        std::optional<engine::Error> DestroyRepeatingTextureCache(
            const std::vector<RepeatingTextureProperties>& textures) const;

        std::optional<engine::Error> BuildSegmentedTextureCache(
            const std::vector<SegmentedTextureProperties>& textures) const;

        std::optional<engine::Error> DestroySegmentedTextureCache(
            const std::vector<SegmentedTextureProperties>& textures) const;

        std::optional<engine::Error> BuildBorderTextureCache(const BorderTextureProperties& properties) const;

        std::optional<engine::Error> DestroyBorderTextureCache(const BorderTextureProperties& properties) const;

        engine::Engine& engine;
        ResourceManager& resourceManager;

        std::unique_ptr<Scene> currentScene;
        std::unordered_map<engine::EntityID, SceneProperties> propertiesCache;
        std::unordered_map<engine::EntityID, ResourceParameters> resourceIndex;

        static constexpr std::string_view sceneIndexDocumentID = "sceneIndex";
    };
}


#endif // SCENE_MANAGER_HPP
