#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>
#include <SFML/System/Vector2.hpp>

namespace farcical {

    struct WindowProperties {
        sf::Vector2u        displayMode;
        sf::Vector2u        sizeInPixels;
        std::string         title;
        sf::Vector2i        position;
        bool                fullscreen;
    };

};

#endif