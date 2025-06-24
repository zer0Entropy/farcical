//
// Created by dgmuller on 5/24/25.
//

#ifndef APP_HPP
#define APP_HPP

#include <expected>
#include <optional>
#include <memory>
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include "error.hpp"
#include "window.hpp"
#include "render.hpp"
#include "input.hpp"
#include "event.hpp"
#include "resource/config.hpp"
#include "resource/manager.hpp"
#include "ui/manager.hpp"
#include "resource/manager.hpp"

namespace farcical {
    namespace game {
        class Game;
    }

    class Engine {
    public:

        enum class Status {
            Uninitialized       =  -2,
            Error               =  -1,
            StoppedSuccessfully =   0,
            IsRunning           =   1
        };

        Engine();
        Engine(Engine& copy) = delete;
        Engine(const Engine& copy) = delete;
        Engine& operator=(const Engine& copy) = delete;
        ~Engine() = default;

        /* Returns a Status code to the caller.
         * Used by the main game loop to detect when to exit,
         * and for error reporting.
         */
        [[nodiscard]]   Status                      GetStatus() const;

        [[nodiscard]]   const WindowProperties&     GetWindowProperties() const;

        /* Init(Game* game) - Returns Error (optionally)
         * Called exactly once per Engine lifetime, *BEFORE* calling Update() for the first time!
         *      (Update() will not function correctly if status != IsRunning)
         * 1. Loads a Config file (as defined in Resources/config.hpp) from $configPath
         *      (hardcoded into this class as "dat/farcical.json")
         * 2. Parses Window properties from that Config and uses them to initialize the window
         * 3. Calls Init() on all Systems
         * 4. Sets status to Engine::Status::IsRunning
         */
        std::optional<Error> Init(game::Game* game);

        /* Updates all Systems */
        void                                        Update();

        /* Stops all Systems */
        void                                        Stop();

        [[nodiscard]]   sf::RenderWindow&           GetWindow() const;

        [[nodiscard]]   ResourceManager&            GetResourceManager() const;

        [[nodiscard]]   ui::Manager&                GetUIManager() const;

        [[nodiscard]]   RenderSystem&               GetRenderSystem() const;

    private:

        Status                                      status;

        static constexpr const std::string_view     configPath{"dat/farcical.json"};
        Config                                      config;

        WindowProperties                            windowConfig;

        std::unique_ptr<sf::RenderWindow>           window;

        ResourceManager                             resourceManager;

        std::unique_ptr<RenderSystem>               renderSystem;

        std::unique_ptr<InputSystem>                inputSystem;

        std::unique_ptr<EventSystem>                eventSystem;

        std::unique_ptr<ui::Manager> uiManager;

        game::Game* game;
    };
};

#endif //APP_HPP
