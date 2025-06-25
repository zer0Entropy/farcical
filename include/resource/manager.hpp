//
// Created by dgmuller on 6/4/25.
//

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <unordered_map>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "resource.hpp"
#include "config.hpp"
#include "../error.hpp"

namespace farcical {

    class ResourceManager final {
    public:
        Resource*                                   GetResource(ResourceID id) const;

        std::optional<Error> LoadResource(ResourceID id,
                                          Resource::Type type,
                                          std::string_view path,
                                          sf::IntRect rect = {{0, 0}, {0, 0}},
                                          bool isRepeating = false);

        std::optional<Error>                        UnloadResource(ResourceID id);

        std::optional<Error> SaveConfig(ResourceID id);

        std::optional<Error>                        SpliceTextures(const std::vector<ResourceID> inputTextureIDs, ResourceID splicedTextureID);

        std::expected<Config*, Error>               GetConfig(ResourceID id) const;
        std::expected<sf::Font*, Error>             GetFont(ResourceID id) const;
        std::expected<sf::Texture*, Error>          GetTexture(ResourceID id) const;

    private:
        std::optional<Error> LoadConfig(ResourceID id, std::string_view path);

        std::optional<Error> LoadFont(ResourceID id, std::string_view path);

        std::optional<Error> LoadTexture(ResourceID id,
                                         std::string_view path,
                                         sf::IntRect rect = sf::IntRect{{0, 0}, {0, 0}},
                                         bool repeat = false);

        std::optional<Error> LoadRepeatingTexture(ResourceID id, std::string_view path,
                                                  sf::IntRect rect = sf::IntRect{{0, 0}, {0, 0}});

        std::unordered_map<ResourceID, Resource> registry;
        std::unordered_map<ResourceID, Config> configs;
        std::unordered_map<ResourceID, sf::Font> fonts;
        std::unordered_map<ResourceID, sf::Texture> textures;
    };

}

#endif //MANAGER_HPP
