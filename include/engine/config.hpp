//
// Created by dgmuller on 7/6/25.
//

#ifndef ENGINE_CONFIG_HPP
#define ENGINE_CONFIG_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include "error.hpp"
#include "../window.hpp"

namespace farcical::engine {
    struct Config {
        WindowProperties windowProperties;
        std::string scenePath;
    };

    std::expected<Config, Error> LoadConfig(const nlohmann::json& json);
}

#endif //ENGINE_CONFIG_HPP
