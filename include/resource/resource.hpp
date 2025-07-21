//
// Created by dgmuller on 6/4/25.
//

#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <string>
#include <SFML/Graphics/Rect.hpp>
#include "../engine/id.hpp"

namespace farcical {

    using ResourceID = std::string;

    using ResourceParameters = std::pair<ResourceID, std::string>;
    using ResourceList = std::vector<std::pair<engine::EntityID, ResourceParameters> >;

    struct ResourceHandle {
        enum class Status {
            Uninitialized,
            IsReady,
            Error
        };

        enum class Type {
            Log,
            JSONDocument,
            Font,
            Texture,
            Sound,
            Music
        };

        ResourceID          id;
        Status              status;
        Type                type;
        std::string         path;
    };

}

#endif //RESOURCE_HPP
