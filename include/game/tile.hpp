//
// Created by dgmuller on 6/26/25.
//

#ifndef TILE_HPP
#define TILE_HPP

#include "terrain.hpp"
#include "creature.hpp"

namespace farcical::game {

    struct Tile {
        Terrain* terrain;
        Creature* occupant;
    };

}

#endif //TILE_HPP
