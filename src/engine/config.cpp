//
// Created by dgmuller on 7/6/25.
//
#include <fstream>
#include "../../include/engine/config.hpp"
#include "../../include/resource/parser.hpp"

std::expected<farcical::engine::Config, farcical::engine::Error> farcical::engine::LoadConfig(
    const nlohmann::json& json) {
    Config config{
        .windowProperties = {
            .displayMode = {0, 0},
            .sizeInPixels = {0, 0},
            .title = "",
            .position = {0, 0},
            .fullscreen = false,
            .detectNativeResolution = false,
        },
        .scenePath = {},
        .logPath = {}
    };
    const auto& findScenePath{json.find("scenePath")};
    if(findScenePath != json.end()) {
        config.scenePath = findScenePath.value().get<std::string>();
    } // if scenePath found

    const auto& findLogPath{json.find("logPath")};
    if(findLogPath != json.end()) {
        config.logPath = findLogPath.value().get<std::string>();
    } // if logPath found

    const auto& findWindow{json.find("window")};
    if(findWindow == json.end()) {
        const std::string failMsg{"Invalid configuration: Window could not be found."};
        return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
    } // if window not found
    const auto& windowPropertiesJSON{findWindow.value()};
    const auto& findWidth{windowPropertiesJSON.find("width")};
    const auto& findHeight{windowPropertiesJSON.find("height")};
    const auto& findTitle{windowPropertiesJSON.find("title")};
    const auto& findFullscreen{windowPropertiesJSON.find("fullscreen")};
    const auto& findDetectNative{windowPropertiesJSON.find("detectNativeResolution")};
    const auto& findPosition{windowPropertiesJSON.find("position")};
    if(findWidth != windowPropertiesJSON.end()) {
        config.windowProperties.displayMode.x = findWidth.value().get<int>();
    } // if width found
    if(findHeight != windowPropertiesJSON.end()) {
        config.windowProperties.displayMode.y = findHeight.value().get<int>();
    } // if height found
    if(findTitle != windowPropertiesJSON.end()) {
        config.windowProperties.title = findTitle.value().get<std::string>();
    } // if title found
    if(findFullscreen != windowPropertiesJSON.end()) {
        config.windowProperties.fullscreen = findFullscreen.value().get<bool>();
    } // if fullscreen found
    if(findDetectNative != windowPropertiesJSON.end()) {
        config.windowProperties.detectNativeResolution = findDetectNative.value().get<bool>();
    } // if detectNativeResolution found
    if(findPosition != windowPropertiesJSON.end()) {
        const auto position{jsonParser::ParsePosition(findPosition.value())};
        if(!position.has_value()) {
            return std::unexpected(position.error());
        }
        const auto& uPos{position.value()};
        config.windowProperties.position = sf::Vector2i{
            static_cast<int>(uPos.x),
            static_cast<int>(uPos.y)
        };
    } // if position found
    return config;
}

std::optional<farcical::engine::Error> farcical::engine::WriteConfig(const Config& config, std::string_view path) {
    const nlohmann::json configJSON{
        {
            "window", {
                {"width", config.windowProperties.displayMode.x},
                {"height", config.windowProperties.displayMode.y},
                {"title", config.windowProperties.title},
                {"fullscreen", config.windowProperties.fullscreen},
                {
                    "position", {
                        {"x", config.windowProperties.position.x},
                        {"y", config.windowProperties.position.y}
                    }
                }
            },
            {"scenePath", config.scenePath},
            {"logPath", config.logPath}
        }
    };
    std::ofstream output{std::string{path}, std::ios_base::out};
    output << configJSON << std::endl;
    return std::nullopt;
}