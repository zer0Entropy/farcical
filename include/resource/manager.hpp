//
// Created by dgmuller on 6/4/25.
//

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <expected>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "config.hpp"
#include "resource.hpp"
#include "../engine/error.hpp"
#include "../engine/system/log.hpp"

namespace farcical {
    class ResourceManager final {
    public:
        ResourceManager();

        ~ResourceManager() = default;

        void AddLogSystem(engine::LogSystem* logSystem);

        void Reset();

        ResourceHandle* GetResourceHandle(ResourceID id) const;

        std::expected<ResourceHandle*, engine::Error> CreateResourceHandle(
            ResourceID id,
            ResourceHandle::Type type,
            std::string_view path);

        std::optional<engine::Error> DestroyResourceHandle(ResourceID id, ResourceHandle::Type type);

        std::expected<engine::Log*, engine::Error> GetLog(ResourceID id);

        std::expected<nlohmann::json*, engine::Error> GetJSONDoc(ResourceID id);

        std::expected<sf::Font*, engine::Error> GetFont(ResourceID id);

        std::expected<sf::Music*, engine::Error> GetMusic(ResourceID id);

        std::expected<sf::Texture*, engine::Error> GetTexture(ResourceID id);

        std::expected<sf::Texture*, engine::Error> GetTexture(TextureProperties properties);

        std::expected<sf::Texture*, engine::Error> GetTexture(RepeatingTextureProperties properties);

        std::expected<sf::Texture*, engine::Error> GetTexture(SegmentedTextureProperties properties);

        std::optional<engine::Error> WriteLog(ResourceID id, const std::vector<std::string>& messages);

        std::optional<engine::Error> AppendToLog(ResourceID id, const std::vector<std::string>& messages);

        std::optional<engine::Error> AppendToLog(engine::Log* log, const std::vector<std::string>& messages);

        std::expected<sf::Texture*, engine::Error> CreateSplicedTexture(
            ResourceID id, const std::vector<ResourceID>& inputTextureIDs);

        std::expected<sf::Texture*, engine::Error> CreateRepeatingTexture(
            ResourceID id,
            const sf::Vector2u& outputSize,
            ResourceID inputID,
            sf::IntRect inputRect = {{0, 0}, {0, 0}});

        std::expected<sf::Texture*, engine::Error> CreateBorderTexture(
            ResourceID id,
            const sf::Vector2u& outputSize,
            const std::vector<ResourceID>& cornerTextureIDs,
            const std::vector<ResourceID>& edgeTextureIDs,
            ResourceID centerTextureID);

    private:
        void RepeatTexture(sf::Texture& input, sf::Texture& output);

        void RepeatSliceHorizontal(sf::Texture& input, sf::Texture& output);

        void RepeatSliceVertical(sf::Texture& input, sf::Texture& output);

        std::unordered_map<ResourceID, ResourceHandle> registry;
        std::unordered_map<ResourceID, engine::Log> logs;
        std::unordered_map<ResourceID, nlohmann::json> jsonDocs;
        std::unordered_map<ResourceID, sf::Font> fonts;
        std::unordered_map<ResourceID, sf::Texture> textures;
        std::unordered_map<ResourceID, sf::Music> musics;

        engine::LogSystem* logSystem;
    };
}

#endif //RESOURCE_MANAGER_HPP