#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <expected>
#include <string>
#include <SFML/System/Vector2.hpp>

namespace farcical {

    struct WindowProperties {
        sf::Vector2u        displayMode;
        sf::Vector2u        sizeInPixels;
        std::string         title;
        sf::Vector2i        position;
        bool                fullscreen;
        bool                detectNativeResolution;
    };

    static constexpr std::expected<WindowProperties, engine::Error> GetWindowProperties(std::string_view description) {
        WindowProperties windowProperties;
        windowProperties.detectNativeResolution = false;
        windowProperties.fullscreen = false;
        const std::string displayString{description};
        const std::string fullscreen{"fullscreen"};
        const auto& findFullscreen{displayString.find(fullscreen)};
        if(findFullscreen != std::string::npos) {
            windowProperties.fullscreen = true;
        } // if fullscreen
        const std::string fullscreenCap{"Fullscreen"};
        const auto& findFullscreenCap{displayString.find(fullscreenCap)};
        if(findFullscreenCap != std::string::npos) {
            windowProperties.fullscreen = true;
        } // if Fullscreen
        const std::string windowed{"windowed"};
        const auto& findWindowed{displayString.find(windowed)};
        if(findWindowed != std::string::npos) {
            windowProperties.fullscreen = false;
        } // if windowed
        const std::string windowedCap{"Windowed"};
        const auto& findWindowedCap{displayString.find(windowedCap)};
        if(findWindowedCap != std::string::npos) {
            windowProperties.fullscreen = false;
        } // if windowedCap

        if(     findFullscreen == std::string::npos
            &&  findFullscreenCap == std::string::npos
            &&  findWindowed == std::string::npos
            &&  findWindowedCap == std::string::npos) {
            const std::string failMsg{"Invalid configuration: Neither windowed nor fullscreen mode specified in display settings."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if !fullscreen && !windowed

        const auto& findNative{displayString.find("native")};
        if(findNative != std::string::npos) {
            windowProperties.detectNativeResolution = true;
        } // if native
        const auto& findNativeCap{displayString.find("Native")};
        if(findNativeCap != std::string::npos) {
            windowProperties.detectNativeResolution = true;
        } // if Native
        const auto& find1920x1280{displayString.find("1920 x 1280")};
        if(find1920x1280 != std::string::npos) {
            windowProperties.displayMode.x = 1920;
            windowProperties.displayMode.y = 1280;
        } // if 1920 x 1280
        const auto& find1280x1024{displayString.find("1280 x 1024")};
        if(find1280x1024 != std::string::npos) {
            windowProperties.displayMode.x = 1280;
            windowProperties.displayMode.y = 1024;
        } // if 1280 x 1024

        if(     findNative == std::string::npos
            &&  findNativeCap == std::string::npos
            &&  find1920x1280 == std::string::npos
            &&  find1280x1024 == std::string::npos) {
            const std::string failMsg{"Invalid configuration: No valid window resolution was specified in the display settings."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // !native && !1920x1280 && !1280x1024

        return windowProperties;
    }

};

#endif