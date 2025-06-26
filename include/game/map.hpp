//
// Created by dgmuller on 6/26/25.
//

#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include "tile.hpp"

namespace farcical::game {

    struct Map {
        int width;
        int height;
        std::vector<Tile> tiles;

        Map() = delete;
        Map(const Map&) = delete;
        Map(Map&) = delete;
        Map(Map&&) = delete;
        Map& operator=(const Map&) = delete;

        Map(int width, int height);
        ~Map();

        Tile* GetTile(int x, int y);
    };

}

#endif //MAP_HPP
