//
// Created by dgmuller on 6/26/25.
//
#include "../../include/game/map.hpp"

farcical::game::Map::Map(int width, int height): width{width}, height{height} {
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            tiles.push_back(Tile{});
        } // for x
    } // for y
}

farcical::game::Map::~Map() {
}

farcical::game::Tile* farcical::game::Map::GetTile(int x, int y) {
    if(x >= 0 && x < width && y >= 0 && y < height) {
        return &tiles.at(y * width + x);
    }
    return nullptr;
}
