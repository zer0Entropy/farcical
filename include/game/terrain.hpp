//
// Created by dgmuller on 6/26/25.
//

#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <string>
#include "../resource/resource.hpp"

namespace farcical::game {
    struct Terrain {
        enum class Type {
            Floor,
            Wall,
            Water,
            Lava
        };

        Type type;
        std::string name;
        bool isWalkable;
        bool isTransparent;
        ResourceID textureID;
    };
}

#endif //TERRAIN_HPP
