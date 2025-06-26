//
// Created by dgmuller on 6/26/25.
//

#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <memory>
#include "map.hpp"

namespace farcical::game {

    struct Level {
        std::string                 name;
        unsigned int                index;
        std::unique_ptr<Map>        map;
    };

}

#endif //LEVEL_HPP
