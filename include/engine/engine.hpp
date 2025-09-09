//
// Created by dgmuller on 5/24/25.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <optional>
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>

#include "config.hpp"
#include "error.hpp"
#include "errorHandler.hpp"
#include "system/event.hpp"
#include "system/input.hpp"
#include "system/log.hpp"
#include "system/music.hpp"
#include "system/render.hpp"
#include "../resource/manager.hpp"
#include "../ui/sceneManager.hpp"

namespace farcical {
    namespace game {
        class Game;
    }

    namespace engine {
        class Engine {
        public:
            enum class Status {
                Uninitialized = -2,
                Error = -1,
                StoppedSuccessfully = 0,
                IsRunning = 1
            };

            Engine() = delete;

            Engine(Engine& copy) = delete;

            Engine(const Engine& copy) = delete;

            Engine(Engine&& move) = delete;

            Engine& operator=(const Engine& copy) = delete;

            explicit Engine(std::string_view configPath);

            ~Engine() = default;

            [[nodiscard]] Status GetStatus() const;

            [[nodiscard]] const Config& GetConfig() const;

            std::expected<Config, Error> CreateConfig();

            std::optional<Error> ApplyConfig(const Config& config);

            std::optional<Error> Init(game::Game* game);

            void Update();

            void Stop();

            [[nodiscard]] ErrorHandler& GetErrorHandler() const;

            [[nodiscard]] sf::RenderWindow& GetWindow() const;

            [[nodiscard]] ResourceManager& GetResourceManager() const;

            [[nodiscard]] ui::SceneManager& GetSceneManager() const;

            [[nodiscard]] EventSystem& GetEventSystem() const;

            [[nodiscard]] InputSystem& GetInputSystem() const;

            [[nodiscard]] LogSystem& GetLogSystem() const;

            [[nodiscard]] MusicSystem& GetMusicSystem() const;

            [[nodiscard]] RenderSystem& GetRenderSystem() const;

        private:
            Status status;

            Config config;
            std::string configPath;

            ErrorHandler errorHandler;

            std::unique_ptr<sf::RenderWindow> window;

            ResourceManager resourceManager;

            std::unique_ptr<ui::SceneManager> sceneManager;

            std::unique_ptr<EventSystem> eventSystem;

            std::unique_ptr<InputSystem> inputSystem;

            std::unique_ptr<LogSystem> logSystem;

            std::unique_ptr<MusicSystem> musicSystem;

            std::unique_ptr<RenderSystem> renderSystem;

            game::Game* game;

            static constexpr std::string_view configDocumentID = "engineConfig";
            static constexpr std::string_view engineLogID = "farcicalLog";

            std::optional<Error> CreateLogSystem();

            std::optional<Error> CreateWindow();

            std::optional<Error> CreateSystems();

            std::optional<Error> InitSystems();
        };
    }
}

#endif //ENGINE_HPP
