//
// Created by dgmuller on 6/3/25.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <expected>
#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../error.hpp"

namespace farcical {

    struct Config {

        enum class Type {
            Application,
            UI,
            Window
        };

        Type type;
        nlohmann::json data;
    };

    std::expected<Config, Error>    LoadConfig(std::string_view path);
    std::optional<Error>            SaveConfig(std::string_view path);

}

#endif //CONFIG_HPP
