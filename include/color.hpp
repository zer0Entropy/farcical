//
// Created by dgmuller on 6/7/25.
//

#ifndef COLOR_HPP
#define COLOR_HPP

#include <string>
#include <SFML/Graphics/Color.hpp>

namespace farcical {

    sf::Color GetColorByName(std::string_view name);

    std::string_view GetColorName(sf::Color color);

}

#endif //COLOR_HPP
