//
// Created by dgmuller on 6/4/25.
//

#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <SFML/Graphics/Rect.hpp>
#include <string>

namespace farcical {

    using ResourceID = std::string;

    struct Resource {

        enum class Status {
            Uninitialized,
            IsReady,
            Error
        };

        enum class Type {
            Config,
            Font,
            Texture,
            Sound,
            Music
        };

        ResourceID          id;
        Status              status;
        Type                type;
        std::string         path;
        sf::IntRect         rect;
    };

}

#endif //RESOURCE_HPP
