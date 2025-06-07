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
#include "resource/config.hpp"
#include "resource/manager.hpp"
#include "ui/manager.hpp"
#include "resource/manager.hpp"

namespace farcical {

    class Application {
    public:

        enum class Status {
            Uninitialized       =  -2,
            Error               =  -1,
            StoppedSuccessfully =   0,
            IsRunning           =   1
        };

        Application();
        Application(Application& app) = delete;
        Application(const Application& app) = delete;
        Application& operator=(const Application& app) = delete;
        ~Application() = default;

        /* Returns a Status code to the caller.
         * Used by the main game loop to detect when to exit,
         * and for error reporting.
         */
        [[nodiscard]]   Status                      GetStatus() const;

        [[nodiscard]]   const WindowProperties&     GetWindowProperties() const;

        /* Loads Config from file & initializes all members */
        std::optional<Error>                        Init();

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

        Config                                      config;

        WindowProperties                            windowConfig;

        std::unique_ptr<sf::RenderWindow>           window;

        ResourceManager                             resourceManager;

        ui::Manager                                 uiManager;

        std::unique_ptr<RenderSystem>               renderSystem;
    };

};

#endif //APP_HPP
