//
// Created by dgmuller on 7/6/25.
//

#include <expected>
#include <nlohmann/json.hpp>
#include <SFML/System/Vector2.hpp>
#include "../engine/error.hpp"

#ifndef PARSER_HPP
#define PARSER_HPP

namespace farcical::jsonParser {

    std::expected<sf::Vector2u, engine::Error> ParsePosition(const nlohmann::json& json);
    std::expected<sf::Vector2u, engine::Error> ParseSize(const nlohmann::json& json);

}

#endif //PARSER_HPP
