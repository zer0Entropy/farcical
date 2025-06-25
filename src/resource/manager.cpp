//
// Created by dgmuller on 6/4/25.
//

#include <SFML/Graphics/Image.hpp>
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
    sf::IntRect rect,
    bool isRepeating) {
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
            bool exists{std::filesystem::exists(path)};
            if(!exists) {
                std::string failMsg{"Failed to load texture file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidPath, failMsg};
            }
            auto registerResult{registry.emplace(id, std::move(resource))};
            if(!registerResult.second) {
                std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
                return Error{Error::Signal::InvalidConfiguration, failMsg};
            }
            if(isRepeating) {
                auto textureResult{
                    textures.emplace(id, sf::Texture{
                        sf::Vector2u{
                            static_cast<unsigned int>(rect.size.x),
                            static_cast<unsigned int>(rect.size.y)},
                    false})};
                if(textureResult.second) {
                    sf::Texture* bigTexture{GetTexture(id).value()};
                    sf::Texture smallTexture{path, false};
                    unsigned int widthInTiles{bigTexture->getSize().x / smallTexture.getSize().x};
                    unsigned int heightInTiles{bigTexture->getSize().y / smallTexture.getSize().y};

                    for(unsigned int y = 0; y < heightInTiles; y++) {
                        for(unsigned int x = 0; x < widthInTiles; x++) {
                            bigTexture->update(smallTexture, sf::Vector2u{
                                x * smallTexture.getSize().x,
                                y * smallTexture.getSize().y});
                        } // for x
                    } // for y

                    if(smallTexture.getSize().x * widthInTiles < bigTexture->getSize().x) {
                        sf::Vector2u horizontalSliceSize{
                            bigTexture->getSize().x - (smallTexture.getSize().x * widthInTiles),
                            smallTexture.getSize().y
                        };
                        sf::Image fullTexture{path};
                        sf::Image horizontalSlice{horizontalSliceSize};
                        sf::IntRect sourceRect{
                            {0, 0},
                            {static_cast<int>(horizontalSliceSize.x), static_cast<int>(horizontalSliceSize.y)},
                        };
                        bool copySuccess{horizontalSlice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
                        if(copySuccess) {
                            sf::Texture hSliceTexture{horizontalSlice};
                            for(unsigned int y = 0; y < heightInTiles; y++) {
                                sf::Vector2u dest{
                                    smallTexture.getSize().x * widthInTiles,
                                    y * smallTexture.getSize().y
                                };
                                bigTexture->update(hSliceTexture, dest);
                            }
                        }
                    } // if smallTexture.width * widthInTiles < bigTexture->width
                    if(smallTexture.getSize().y * heightInTiles < bigTexture->getSize().y) {
                        sf::Vector2u verticalSliceSize{
                            smallTexture.getSize().x,
                            bigTexture->getSize().y - (smallTexture.getSize().y * heightInTiles)
                        };
                        sf::Image fullTexture{path};
                        sf::Image verticalSlice{verticalSliceSize};
                        sf::IntRect sourceRect{
                            {0, 0},
                            {static_cast<int>(verticalSliceSize.x), static_cast<int>(verticalSliceSize.y)},
                        };
                        bool copySuccess{verticalSlice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
                        if(copySuccess) {
                            sf::Texture vSliceTexture{verticalSlice};
                            for(unsigned int x = 0; x < widthInTiles; x++) {
                                sf::Vector2u dest{
                                    x * smallTexture.getSize().x,
                                    smallTexture.getSize().y * heightInTiles,
                                };
                                bigTexture->update(vSliceTexture, dest);
                            }
                        }
                    } // if smallTexture.height * heightInTiles < bigTexture->height
                    if(smallTexture.getSize().x * widthInTiles < bigTexture->getSize().x
                    && smallTexture.getSize().y * heightInTiles < bigTexture->getSize().y) {
                        sf::Vector2u sliceSize{
                            bigTexture->getSize().x - (smallTexture.getSize().x * widthInTiles),
                            bigTexture->getSize().y - (smallTexture.getSize().y * heightInTiles)
                        };
                        sf::Image fullTexture{path};
                        sf::Image slice{sliceSize};
                        sf::IntRect sourceRect{
                                {0, 0},
                                {static_cast<int>(sliceSize.x), static_cast<int>(sliceSize.y)},
                        };
                        bool copySuccess{slice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
                        if(copySuccess) {
                            sf::Texture sliceTexture{slice};
                            sf::Vector2u dest{
                                smallTexture.getSize().x * widthInTiles,
                                smallTexture.getSize().y * heightInTiles,
                            };
                            bigTexture->update(sliceTexture, dest);
                        }
                    } // if smallTexture.width * widthInTiles < bigTexture->width &&
                    //      smallTexture.height * heightInTiles < bigTexture->height


                } // if creating texture succeeded
            } // isRepeating
            else {
                auto textureResult{textures.emplace(id, sf::Texture{path, false, rect})};
                if(!textureResult.second) {
                    std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
                    return Error{Error::Signal::InvalidConfiguration, failMsg};
                } // if loading texture failed
            } // !isRepeating
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
        if(totalSize.y < texture.getSize().y) {
            totalSize.y = texture.getSize().y;
        }
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
