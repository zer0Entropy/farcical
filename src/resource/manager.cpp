//
// Created by dgmuller on 6/4/25.
//

#include "../../include/resource/manager.hpp"

farcical::Resource* farcical::ResourceManager::GetResource(ResourceID id) const {
    const auto resourceIter{registry.find(id)};
    if(resourceIter != registry.end()) {
        return const_cast<Resource*>(&resourceIter->second);
    }
    return nullptr;
}

std::optional<farcical::Error> farcical::ResourceManager::LoadResource(
    ResourceID id,
    Resource::Type type,
    std::string_view path,
    sf::IntRect rect ) {
    Resource resource{
        .id = id,
        .status = Resource::Status::Uninitialized,
        .type = type,
        .path = std::string{path},
        .rect = rect
    };
    switch(type) {
        case Resource::Type::Config: {
            auto result{LoadConfig(path)};
            if(!result.has_value()) {
                return result.error();
            }
            auto registerResult{registry.emplace(id, std::move(resource))};
            if(!registerResult.second) {
                std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidConfiguration, failMsg};
            }
            auto configResult{configs.emplace(id, result.value())};
            if(!configResult.second) {
                std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidConfiguration, failMsg};
            }
        } break;
        case Resource::Type::Font: {
            bool success{std::filesystem::exists(path)};
            if(!success) {
                std::string failMsg{"Failed to load font file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidPath, failMsg};
            }
            auto registerResult{registry.emplace(id, std::move(resource))};
            if(!registerResult.second) {
                std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidConfiguration, failMsg};
            }
            auto fontResult{fonts.emplace(id, sf::Font{path})};
            if(!fontResult.second) {
                std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidConfiguration, failMsg};
            }
        } break;
        case Resource::Type::Texture: {
            bool success{std::filesystem::exists(path)};
            if(!success) {
                std::string failMsg{"Failed to load font file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidPath, failMsg};
            }
            auto registerResult{registry.emplace(id, std::move(resource))};
            if(!registerResult.second) {
                std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidConfiguration, failMsg};
            }
            auto textureResult{textures.emplace(id, sf::Texture{path, false, rect})};
            if(!textureResult.second) {
                std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidConfiguration, failMsg};
            }
        } break;
        case Resource::Type::Sound:
            break;
        case Resource::Type::Music:
            break;
    }
    return std::nullopt;
}

std::optional<farcical::Error> farcical::ResourceManager::UnloadResource(ResourceID id) {
    auto resourceIter{registry.find(id)};
    if(resourceIter == registry.end()) {
        std::string failMsg{"Resource not found: " + id};
        return Error{Error::Signal::ResourceNotFound, failMsg};
    }
    Resource::Type type{resourceIter->second.type};
    registry.erase(resourceIter);
    switch(type) {
        case Resource::Type::Config: {
            configs.erase(id);
        } break;
        case Resource::Type::Font: {
            fonts.erase(id);
        } break;
        case Resource::Type::Texture: {
            textures.erase(id);
        } break;
        case Resource::Type::Sound: {

        } break;
        case Resource::Type::Music: {

        } break;
    }

    return std::nullopt;
}

std::optional<farcical::Error> farcical::ResourceManager::SpliceTextures(const std::vector<ResourceID> inputTextureIDs, ResourceID splicedTextureID) {

    sf::Vector2u totalSize{0, 0};
    // Compute the total size of all textures spliced together (horizontally)
    for(const auto& textureID : inputTextureIDs) {
        auto requestTexture{GetTexture(textureID)};
        if(!requestTexture.has_value()) {
            return requestTexture.error();
        }
        sf::Texture& texture{*requestTexture.value()};
        totalSize.x += texture.getSize().x;
        totalSize.y = texture.getSize().y;
    }
    // Create a new texture of the appropriate size
    sf::Texture splicedTexture{totalSize};
    sf::Vector2u dest{0, 0};
    // Copy each inputTexture into the splicedTexture
    for(const auto& textureID : inputTextureIDs) {
        auto requestTexture{GetTexture(textureID)};
        if(!requestTexture.has_value()) {
            return requestTexture.error();
        }
        sf::Texture& texture{*requestTexture.value()};
        splicedTexture.update(texture, dest);
        dest.x += texture.getSize().x;
    }
    // Enter splicedTexture into Resource registry & texture store
    registry.emplace(splicedTextureID, Resource{
        .id = splicedTextureID,
        .status = Resource::Status::IsReady,
        .type = Resource::Type::Texture,
        .path = "",
        .rect = sf::IntRect{sf::Vector2i{0, 0}, sf::Vector2i{static_cast<int>(totalSize.x), static_cast<int>(totalSize.y)}}});
    textures.emplace(splicedTextureID, splicedTexture);
    return std::nullopt;

}

std::expected<farcical::Config*, farcical::Error> farcical::ResourceManager::GetConfig(ResourceID id) const {
    const auto configIter{configs.find(id)};
    if(configIter != configs.end()) {
        return const_cast<Config*>(&configIter->second);
    }
    std::string failMsg{"Resource not found: " + id};
    return std::unexpected{Error{Error::Signal::ResourceNotFound, failMsg}};
}

std::expected<sf::Font*, farcical::Error> farcical::ResourceManager::GetFont(ResourceID id) const {
    const auto fontIter{fonts.find(id)};
    if(fontIter != fonts.end()) {
        return const_cast<sf::Font*>(&fontIter->second);
    }
    std::string failMsg{"Resource not found: " + id};
    return std::unexpected{Error{Error::Signal::ResourceNotFound, failMsg}};
}

std::expected<sf::Texture*, farcical::Error> farcical::ResourceManager::GetTexture(ResourceID id) const {
    const auto textureIter{textures.find(id)};
    if(textureIter != textures.end()) {
        return const_cast<sf::Texture*>(&textureIter->second);
    }
    std::string failMsg{"Resource not found: " + id};
    return std::unexpected{Error{Error::Signal::ResourceNotFound, failMsg}};
}
