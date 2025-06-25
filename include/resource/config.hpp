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

}

#endif //CONFIG_HPP
