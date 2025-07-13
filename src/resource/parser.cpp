//
// Created by dgmuller on 7/6/25.
//

#include "../../include/resource/parser.hpp"

std::expected<sf::Vector2u, farcical::engine::Error> farcical::jsonParser::ParsePosition(const nlohmann::json& json) {
    sf::Vector2u position{0, 0};
    const auto& findX{json.find("x")};
    const auto& findY{json.find("y")};
    if(findX != json.end() && findY != json.end()) {
        position.x = *findX;
        position.y = *findY;
    }
    else {
        const std::string failMsg{"Invalid configuration: invalid position"};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    }
    return position;
}

std::expected<sf::Vector2u, farcical::engine::Error> farcical::jsonParser::ParseSize(const nlohmann::json& json) {
    sf::Vector2u size{0, 0};
    const auto& findWidth{json.find("width")};
    const auto& findHeight{json.find("height")};
    if(findWidth != json.end() && findHeight != json.end()) {
        size.x = *findWidth;
        size.y = *findHeight;
    }
    else {
        const std::string failMsg{"Invalid configuration: invalid size"};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    }
    return size;
}
