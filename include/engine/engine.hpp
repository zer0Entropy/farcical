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
#include "render.hpp"
#include "input.hpp"
#include "event.hpp"
#include "../resource/manager.hpp"

namespace farcical {

    namespace game {
        class Game;
    }

    namespace engine {

        class Engine {
        public:

            enum class Status {
                Uninitialized       =  -2,
                Error               =  -1,
                StoppedSuccessfully =   0,
                IsRunning           =   1
            };

            Engine() = delete;
            Engine(Engine& copy) = delete;
            Engine(const Engine& copy) = delete;
            Engine(Engine&& move) = delete;
            Engine& operator=(const Engine& copy) = delete;
            explicit Engine(std::string_view configPath);
            ~Engine() = default;

            [[nodiscard]]   Status                      GetStatus() const;

            [[nodiscard]]   const Config&               GetConfig() const;

            std::optional<Error> Init(game::Game* game);

            void                                        Update();

            void                                        Stop();

            [[nodiscard]]   sf::RenderWindow&           GetWindow() const;

            [[nodiscard]]   ResourceManager&            GetResourceManager() const;

            [[nodiscard]]   RenderSystem&               GetRenderSystem() const;

            [[nodiscard]]   InputSystem&                GetInputSystem() const;

            [[nodiscard]]   EventSystem&                GetEventSystem() const;

        private:

            Status                                      status;

            Config                                      config;

            std::unique_ptr<sf::RenderWindow>           window;

            ResourceManager                             resourceManager;

            std::unique_ptr<RenderSystem>               renderSystem;

            std::unique_ptr<InputSystem>                inputSystem;

            std::unique_ptr<EventSystem>                eventSystem;

            game::Game* game;

            static constexpr std::string_view           configDocumentID = "engineConfigJSON";
        };

    }
}

#endif //ENGINE_HPP
