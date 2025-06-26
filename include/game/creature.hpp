//
// Created by dgmuller on 6/26/25.
//

#ifndef CREATURE_HPP
#define CREATURE_HPP

#include "../resource/resource.hpp"

namespace farcical::game {
    struct Creature {
        std::string name;
        ResourceID textureID;
    };
}

#endif //CREATURE_HPP
