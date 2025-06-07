//
// Created by dgmuller on 6/3/25.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <SFML/Graphics/Rect.hpp>

#include "SFML/Graphics/Color.hpp"

namespace farcical::ui {

    struct TextureSource {
        std::string path;
        sf::IntRect rect;
    };

    struct Config {
        TextureSource   buttonTexture;
        float           scale;
        std::string     fontPath;
        unsigned int    fontSize;
        sf::Color       fontColor;
        sf::Color       outlineColor;
        float           outlineThickness;
    };

}

#endif //CONFIG_HPP
