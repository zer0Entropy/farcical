//
// Created by dgmuller on 6/4/25.
//

#include <fstream>
#include <SFML/Graphics/Image.hpp>
#include "../../include/resource/manager.hpp"

#include "../../include/color.hpp"

farcical::ResourceHandle* farcical::ResourceManager::GetResourceHandle(ResourceID id) const {
    const auto resourceIter{registry.find(id)};
    if(resourceIter != registry.end()) {
        return const_cast<ResourceHandle*>(&resourceIter->second);
    }
    return nullptr;
}

std::expected<farcical::ResourceHandle*, farcical::engine::Error> farcical::ResourceManager::CreateResourceHandle(
    ResourceID id, ResourceHandle::Type type, std::string_view path) {
    const auto& createResourceResult{
        registry.emplace(id, ResourceHandle{id, ResourceHandle::Status::Uninitialized, type, std::string{path}})
    };
    if(!createResourceResult.second) {
        const std::string failMsg{"Failed to create resource " + id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    }
    return &createResourceResult.first->second;
}

std::optional<farcical::engine::Error> farcical::ResourceManager::DestroyResourceHandle(ResourceID id, ResourceHandle::Type type) {
    switch(type) {
        case ResourceHandle::Type::JSONDocument: {
            const auto& findJSONDoc{jsonDocs.find(id)};
            if(findJSONDoc != jsonDocs.end()) {
                jsonDocs.erase(findJSONDoc);
            }
        } break;
        case ResourceHandle::Type::Font: {
            const auto& findFont{fonts.find(id)};
            if(findFont != fonts.end()) {
                fonts.erase(findFont);
            }
        } break;
        case ResourceHandle::Type::Texture: {
            const auto& findTexture{textures.find(id)};
            if(findTexture != textures.end()) {
                textures.erase(findTexture);
            }
        } break;
        case ResourceHandle::Type::Sound: {

        } break;
        case ResourceHandle::Type::Music: {

        } break;
    }
    const auto& findResource{registry.find(id)};
    if(findResource != registry.end()) {
        registry.erase(findResource);
    }
    return std::nullopt;
}

std::expected<nlohmann::json*, farcical::engine::Error> farcical::ResourceManager::GetJSONDoc(ResourceID id) {
    ResourceHandle* handle{GetResourceHandle(id)};
    // If a ResourceHandle with this ResourceID has not been created previously, return Error{ResourceNotFound}
    if(!handle) {
        const std::string failMsg{"Resource not found: " + id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if handle does not exist
    if(handle->status == ResourceHandle::Status::IsReady) {
        // Confirm this JSONDocument has already been created
        const auto& jsonDocIter{jsonDocs.find(id)};
        if(jsonDocIter != jsonDocs.end()) {
            return &jsonDocIter->second;
        } // if jsonDocument found
        // If it hasn't, mark the ResourceHandle with an Error status & return Error{ResourceNotFound}
        handle->status = ResourceHandle::Status::Error;
        const std::string failMsg{"Resource not found: " + id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if ResourceHandle is marked IsReady
    if(handle->status == ResourceHandle::Status::Uninitialized) {
        // Create the jsonDocument and load it from file
        const auto& jsonDocIter{jsonDocs.emplace(id, nlohmann::json{}).first};
        std::ifstream inputFromFile{handle->path};
        if(inputFromFile.good()) {
            jsonDocIter->second = nlohmann::json::parse(inputFromFile);
        } // if inputFromFile.good()
        // If the file was successfully opened, close it
        if(inputFromFile.is_open()) {
            inputFromFile.close();
            handle->status = ResourceHandle::Status::IsReady;
            //const std::string dump{jsonDocIter->second.dump()};
            return &jsonDocIter->second;
        } // if inputFromFile.is_open()
        // Otherwise, return Error{InvalidPath}
        const std::string failMsg{"Invalid path: Could not open JSONDocument at " + handle->path + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
    } // if ResourceHandle is marked Uninitialized
    // If we reach this code path, an Error has occurred!
    const std::string failMsg{"Resource not found: " + id + "."};
    return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
}

std::expected<sf::Font*, farcical::engine::Error> farcical::ResourceManager::GetFont(ResourceID id){
    ResourceHandle* handle{GetResourceHandle(id)};
    // If a ResourceHandle with this ResourceID has not been created previously, return Error{ResourceNotFound}
    if(!handle) {
        const std::string failMsg{"Resource not found: " + id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if handle does not exist
    if(handle->status == ResourceHandle::Status::IsReady) {
        // Confirm this Font has already been created
        const auto& fontIter{fonts.find(id)};
        if(fontIter != fonts.end()) {
            return &fontIter->second;
        } // if Font found
        // If it hasn't, mark the ResourceHandle with an Error status & return Error{ResourceNotFound}
        else {
            handle->status = ResourceHandle::Status::Error;
            const std::string failMsg{"Resource not found: " + id + "."};
            return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
        } // else Font not found
    } // if ResourceHandle is marked IsReady
    else if(handle->status == ResourceHandle::Status::Uninitialized) {
        // Create the Font and load it from file
        const auto& fontResult{fonts.emplace(id, sf::Font{handle->path})};
        if(!fontResult.second) {
            const std::string failMsg{"Invalid path: Could not open Font at " + handle->path + "."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
        } // if fontResult.second == false
        else {
            handle->status = ResourceHandle::Status::IsReady;
            return &fontResult.first->second;
        } // else fontResult.second == true
    } // else if ResourceHandle is marked Uninitialized
    // If we reach this code path, an Error has occurred!
    const std::string failMsg{"Resource not found: " + id};
    return std::unexpected{engine::Error{engine::Error::Signal::ResourceNotFound, failMsg}};
}

std::expected<sf::Texture*, farcical::engine::Error> farcical::ResourceManager::GetTexture(ResourceID id) {
    ResourceHandle* handle{GetResourceHandle(id)};
    // If a ResourceHandle with this ResourceID has not been created previously, return Error{ResourceNotFound}
    if(!handle) {
        const std::string failMsg{"Resource not found: " + id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if handle does not exist
    if(handle->status == ResourceHandle::Status::IsReady) {
        // Confirm this Texture has already been created
        const auto& textureIter{textures.find(id)};
        if(textureIter != textures.end()) {
            return &textureIter->second;
        } // if Texture found
        // If it hasn't, mark the ResourceHandle with an Error status & return Error{ResourceNotFound}
        handle->status = ResourceHandle::Status::Error;
        const std::string failMsg{"Resource not found: " + id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if ResourceHandle is marked IsReady
    if(handle->status == ResourceHandle::Status::Uninitialized) {
        // Create the Texture and load it from file
        const auto& textureResult{textures.emplace(id, sf::Texture{handle->path, false})};
        if(!textureResult.second) {
            const std::string failMsg{"Invalid path: Could not open Texture at " + handle->path + "."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
        } // if textureResult.second == false
        handle->status = ResourceHandle::Status::IsReady;
        return &textureResult.first->second;
    } // else if ResourceHandle is marked Uninitialized
    // If we reach this code path, an Error has occurred!
    const std::string failMsg{"Resource not found: " + id};
    return std::unexpected{engine::Error{engine::Error::Signal::ResourceNotFound, failMsg}};
}

std::expected<sf::Texture*, farcical::engine::Error> farcical::ResourceManager::GetTexture(TextureProperties properties) {
    ResourceHandle* handle{GetResourceHandle(properties.id)};
    // If a ResourceHandle with this ResourceID has not been created previously, return Error{ResourceNotFound}
    if(!handle) {
        const std::string failMsg{"Resource not found: " + properties.id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if handle does not exist
    if(handle->status == ResourceHandle::Status::IsReady) {
        // Confirm this Texture has already been created
        const auto& textureIter{textures.find(properties.id)};
        if(textureIter != textures.end()) {
            return &textureIter->second;
        } // if Texture found
        // If it hasn't, mark the ResourceHandle with an Error status & return Error{ResourceNotFound}
        handle->status = ResourceHandle::Status::Error;
        const std::string failMsg{"Resource not found: " + properties.id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if ResourceHandle is marked IsReady
    if(handle->status == ResourceHandle::Status::Uninitialized) {
        if(properties.inputRect.size.x > 0 && properties.inputRect.size.y > 0) {
            const auto& textureResult{textures.emplace(properties.id, sf::Texture{handle->path, false, properties.inputRect})};
            if(!textureResult.second) {
                const std::string failMsg{"Invalid path: Could not open Texture at " + handle->path + "."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
            } // if textureResult.second == false
            handle->status = ResourceHandle::Status::IsReady;
            return &textureResult.first->second;
        } // if inputRect.size > 0
        else {
            const auto& textureResult{textures.emplace(properties.id, sf::Texture{handle->path, false})};
            if(!textureResult.second) {
                const std::string failMsg{"Invalid path: Could not open Texture at " + handle->path + "."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
            } // if textureResult.second == false
            handle->status = ResourceHandle::Status::IsReady;
            return &textureResult.first->second;
        } // else inputRect.size == 0
    } // else if ResourceHandle is marked Uninitialized
    // If we reach this code path, an Error has occurred!
    const std::string failMsg{"Resource not found: " + properties.id};
    return std::unexpected{engine::Error{engine::Error::Signal::ResourceNotFound, failMsg}};
}

std::expected<sf::Texture*, farcical::engine::Error> farcical::ResourceManager::GetTexture(RepeatingTextureProperties properties) {
    ResourceHandle* inputHandle{GetResourceHandle(properties.inputID)};
    ResourceHandle* outputHandle{GetResourceHandle(properties.outputID)};
    // If ResourceHandles for both inputID and outputID have not been created previously, return Error{ResourceNotFound}
    if(!inputHandle) {
        const std::string failMsg{"Resource not found: " + properties.inputID + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if inputHandle does not exist
    if(!outputHandle) {
        const std::string failMsg{"Resource not found: " + properties.outputID + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if outputHandle does not exist
    if(outputHandle->status == ResourceHandle::Status::IsReady) {
        // Confirm this Texture has already been created
        const auto& textureIter{textures.find(properties.outputID)};
        if(textureIter != textures.end()) {
            return &textureIter->second;
        } // if Texture found
        // If it hasn't, mark outputHandle with an Error status & return Error{ResourceNotFound}
        outputHandle->status = ResourceHandle::Status::Error;
        const std::string failMsg{"Resource not found: " + properties.outputID + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if outputHandle is marked IsReady
    if(outputHandle->status == ResourceHandle::Status::Uninitialized) {
        if(inputHandle->status == ResourceHandle::Status::Uninitialized) {
            TextureProperties inputProperties;
            inputProperties.id = properties.inputID;
            inputProperties.path = properties.path;
            inputProperties.scale = properties.scale;
            inputProperties.inputRect = properties.inputRect;
            const auto& loadInputTexture{GetTexture(inputProperties)};
            if(loadInputTexture.has_value()) {
                inputHandle->status = ResourceHandle::Status::IsReady;
            } // if loadInputTexture == success
        } // if inputHandle is Uninitialized
        if(inputHandle->status == ResourceHandle::Status::IsReady) {
            const auto& outputTexture{CreateRepeatingTexture(properties.outputID, properties.outputSize, properties.inputID, properties.inputRect)};
            if(outputTexture.has_value()) {
                outputHandle->status = ResourceHandle::Status::IsReady;
                return outputTexture.value();
            } // if outputTexture == success
        } // if inputHandle is marked IsReady
    } // if outputHandle is Uninitialized
    // If we reach this code path, an Error has occurred!
    const std::string failMsg{"Resource not found: " + properties.outputID};
    return std::unexpected{engine::Error{engine::Error::Signal::ResourceNotFound, failMsg}};
}

std::expected<sf::Texture*, farcical::engine::Error> farcical::ResourceManager::GetTexture(SegmentedTextureProperties properties) {
    ResourceHandle* handle{GetResourceHandle(properties.id)};
    // If a ResourceHandle with this ResourceID has not been created previously, return Error{ResourceNotFound}
    if(!handle) {
        const std::string failMsg{"Resource not found: " + properties.id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if handle does not exist
    if(handle->status == ResourceHandle::Status::IsReady) {
        // Confirm this Texture has already been created
        const auto& textureIter{textures.find(properties.id)};
        if(textureIter != textures.end()) {
            return &textureIter->second;
        } // if Texture found
        // If it hasn't, mark the ResourceHandle with an Error status & return Error{ResourceNotFound}
        handle->status = ResourceHandle::Status::Error;
        const std::string failMsg{"Resource not found: " + properties.id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if ResourceHandle is marked IsReady
    if(handle->status == ResourceHandle::Status::Uninitialized) {
        std::vector<ResourceID> segmentList;
        for(const auto& segment: properties.segments) {
            // Create the Texture and load it from file
            const auto& segmentResult{textures.emplace(segment.id, sf::Texture{segment.path, false, segment.inputRect})};
            if(!segmentResult.second) {
                const std::string failMsg{"Invalid path: Could not open Texture at " + segment.path + "."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
            } // if segmentResult.second == false
            segmentList.push_back(segment.id);
        } // for each segment in segmentedTexture
        const auto& createSegmentedTexture{CreateSplicedTexture(properties.id, segmentList)};
        if(createSegmentedTexture.has_value()) {
            handle->status = ResourceHandle::Status::IsReady;
            return createSegmentedTexture.value();
        } // if createSegmentedTexture == success
    } // else if ResourceHandle is marked Uninitialized
    // If we reach this code path, an Error has occurred!
    const std::string failMsg{"Resource not found: " + properties.id};
    return std::unexpected{engine::Error{engine::Error::Signal::ResourceNotFound, failMsg}};
}

std::expected<sf::Texture*, farcical::engine::Error> farcical::ResourceManager::CreateSplicedTexture(
    ResourceID id, std::vector<ResourceID> inputTextureIDs) {

    sf::Vector2u totalSize{0, 0};
    // Compute the total size of all textures spliced together (horizontally)
    for(const auto& textureID : inputTextureIDs) {
        const auto& requestTexture{GetTexture(textureID)};
        // If requested Texture not found, return Error
        if(!requestTexture.has_value()) {
            return std::unexpected(requestTexture.error());
        }
        sf::Texture& texture{*requestTexture.value()};
        // Add requested Texture's width to the totalSize
        totalSize.x += texture.getSize().x;
        // If totalSize is shorter than requested Texture, set its height to requested Texture's
        if(totalSize.y < texture.getSize().y) {
            totalSize.y = texture.getSize().y;
        }
    }

    // Create a new texture of the appropriate size
    const auto& createTexture{textures.emplace(id, sf::Texture{totalSize})};
    if(!createTexture.second) {
        const std::string failMsg{"Invalid configuration: Failed to create splicedTexture with id=" + id + "."};
        return std::unexpected{engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg}};
    }
    sf::Texture& splicedTexture{createTexture.first->second};
    sf::Vector2u dest{0, 0};
    // Copy each inputTexture into the splicedTexture
    for(const auto& textureID : inputTextureIDs) {
        auto requestTexture{GetTexture(textureID)};
        // If requested Texture not found, return Error
        if(!requestTexture.has_value()) {
            return std::unexpected(requestTexture.error());
        }
        sf::Texture& texture{*requestTexture.value()};
        // Copy inputTexture into splicedTexture at dest
        splicedTexture.update(texture, dest);
        dest.x += texture.getSize().x;
    }

    return &splicedTexture;
}

std::expected<sf::Texture*, farcical::engine::Error> farcical::ResourceManager::CreateRepeatingTexture(
    ResourceID id, sf::Vector2u outputSize, ResourceID inputID, sf::IntRect inputRect) {

    ResourceHandle* inputHandle{GetResourceHandle(inputID)};
    // If a ResourceHandle with inputID has not been created previously, return Error{ResourceNotFound}
    if(!inputHandle) {
        const std::string failMsg{"Resource not found: " + inputID + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if handle does not exist

    // Create a blank canvas, which we will fill with repeating tiles.
    const auto& createTextureResult{
        textures.emplace(id, sf::Texture{outputSize, false})
    };
    if(createTextureResult.second) {
        sf::Texture* outputTexture{nullptr};
        // Retrieve the blank Texture we just created
        const auto& findOutputTexture{textures.find(id)};
        if(findOutputTexture != textures.end()) {
            outputTexture = &findOutputTexture->second;
        }
        else {
            const std::string failMsg{"Resource not found: " + id + "."};
            return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
        }

        // Retrieve the inputTexture
        sf::Texture* inputTexture{nullptr};
        if(inputRect.size.x > 0 && inputRect.size.y > 0) {
            TextureProperties properties;
            properties.id = inputID;
            properties.path = inputHandle->path;
            properties.scale = 1.0f;
            properties.inputRect = inputRect;
            const auto& requestInputTexture{GetTexture(properties)};
            if(requestInputTexture.has_value()) {
                inputTexture = requestInputTexture.value();
            }
            else {
                const std::string failMsg{"Resource not found: " + inputID + "."};
                return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
            }
        }  // if inputRect.size > 0
        else {
            const auto& requestInputTexture{GetTexture(inputID)};
            if(requestInputTexture.has_value()) {
                inputTexture = requestInputTexture.value();
            }
            else {
                const std::string failMsg{"Resource not found: " + inputID + "."};
                return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
            }
        } // else inputRect.size <= 0

        // Fill outputTexture with inputTextures
        RepeatTexture(*inputTexture, *outputTexture);

        const unsigned int widthInTiles{outputTexture->getSize().x / inputTexture->getSize().x};
        const unsigned int heightInTiles{outputTexture->getSize().y / inputTexture->getSize().y};

        // If we have a fractional tile on the x-axis...
        if(inputTexture->getSize().x * widthInTiles < outputTexture->getSize().x) {
            RepeatSliceHorizontal(*inputTexture, *outputTexture);
        } // if fractional tile on x-axis

        // If we have a fractional tile on the y-axis...
        if(inputTexture->getSize().y * heightInTiles < outputTexture->getSize().y) {
            RepeatSliceVertical(*inputTexture, *outputTexture);
        } // if fractional tile on y-axis

        // If we have a fractional tile on both the x-axis AND the y-axis...
        if(inputTexture->getSize().x * widthInTiles < outputTexture->getSize().x
        && inputTexture->getSize().y * heightInTiles < outputTexture->getSize().y) {
            const sf::Vector2u sliceSize{
                outputTexture->getSize().x - (inputTexture->getSize().x * widthInTiles),
                outputTexture->getSize().y - (inputTexture->getSize().y * heightInTiles)
            };
            sf::Image slice{sliceSize};
            const sf::IntRect sourceRect{
                    {0, 0},
                    {static_cast<int>(sliceSize.x), static_cast<int>(sliceSize.y)},
                };
            sf::Image inputImage{inputTexture->copyToImage()};
            bool copySuccess{slice.copy(inputImage, sf::Vector2u{0, 0}, sourceRect)};
            if(copySuccess) {
                // Copy that slice over at the end of the last column/row
                const sf::Texture sliceTexture{slice};
                const sf::Vector2u dest{
                    inputTexture->getSize().x * widthInTiles,
                    inputTexture->getSize().y * heightInTiles,
                };
                outputTexture->update(sliceTexture, dest);
            } // if copySuccess
        } // If we have a fractional tile on both the x-axis AND the y-axis
        return outputTexture;
    } // if createTextureResult.second
    const std::string failMsg{"Invalid configuration: Failed to create Texture " + id + "."};
    return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
}

void farcical::ResourceManager::RepeatTexture(sf::Texture& input, sf::Texture& output) {
    // Calculate # of tiles that fit in outputTexture, then copy inputTexture into outputTexture that many times
    const unsigned int widthInTiles{output.getSize().x / input.getSize().x};
    const unsigned int heightInTiles{output.getSize().y / input.getSize().y};
    for(unsigned int y = 0; y < heightInTiles; y++) {
        for(unsigned int x = 0; x < widthInTiles; x++) {
            output.update(input, sf::Vector2u{x * input.getSize().x,y * input.getSize().y});
        } // for x
    } // for y
}

void farcical::ResourceManager::RepeatSliceHorizontal(sf::Texture& input, sf::Texture& output) {
    // How many whole inputs have been tiled into output?
    const unsigned int widthInTiles{output.getSize().x / input.getSize().x};
    const unsigned int heightInTiles{output.getSize().y / input.getSize().y};

    // Calculate the remainder on the x-axis
    const sf::Vector2u sliceSize{
        output.getSize().x - (input.getSize().x * widthInTiles),
        input.getSize().y
    };

    // Create inputImage from inputTexture
    sf::Image inputImage{input.copyToImage()};

    // Grab a "slice" of the inputImage of the appropriate size
    sf::Image slice{sliceSize};
    const sf::IntRect sourceRect{
        {0, 0},
        {static_cast<int>(sliceSize.x), static_cast<int>(sliceSize.y)},
    };
    bool copySuccess{slice.copy(inputImage, sf::Vector2u{0, 0}, sourceRect)};
    if(copySuccess) {
        // Copy that slice at the end of each row
        const sf::Texture sliceTexture{slice};
        for(unsigned int y = 0; y < heightInTiles; ++y) {
            const sf::Vector2u dest{
                input.getSize().x * widthInTiles,
                y * input.getSize().y
            };
            output.update(sliceTexture, dest);
        } // for each row
    } // if copySuccess
}

void farcical::ResourceManager::RepeatSliceVertical(sf::Texture& input, sf::Texture& output) {
    // How many whole inputs have been tiled into output?
    const unsigned int widthInTiles{output.getSize().x / input.getSize().x};
    const unsigned int heightInTiles{output.getSize().y / input.getSize().y};

    // Calculate the remainder on the y-axis
    const sf::Vector2u sliceSize{
        input.getSize().x,
        output.getSize().y - (input.getSize().x * heightInTiles)
    };

    // Create inputImage from inputTexture
    sf::Image inputImage{input.copyToImage()};

    // Grab a "slice" of the inputImage of the appropriate size
    sf::Image slice{sliceSize};
    const sf::IntRect sourceRect{
            {0, 0},
            {static_cast<int>(sliceSize.x), static_cast<int>(sliceSize.y)},
        };
    bool copySuccess{slice.copy(inputImage, sf::Vector2u{0, 0}, sourceRect)};
    if(copySuccess) {
        // Copy that slice at the end of each row
        const sf::Texture sliceTexture{slice};
        for(unsigned int x = 0; x < widthInTiles; ++x) {
            const sf::Vector2u dest{
                x * input.getSize().x,
                input.getSize().y * heightInTiles
            };
            output.update(sliceTexture, dest);
        } // for each row
    } // if copySuccess
}

/*
std::optional<farcical::Error> farcical::ResourceManager::LoadFont(Resource* resource) {
    const bool exists{std::filesystem::exists(resource->path)};
    if(!exists) {
        const std::string failMsg{"Failed to load font file: " + std::string{resource->path} + "."};
        return Error{Error::Signal::InvalidPath, failMsg};
    }
    const auto& fontResult{fonts.emplace(resource->id, sf::Font{resource->path})};
    if(!fontResult.second) {
        const std::string failMsg{"Invalid configuration (file: " + std::string{resource->path} + "."};
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    }
    resource->status = Resource::Status::IsReady;
    return std::nullopt;
}

std::optional<farcical::Error> farcical::ResourceManager::LoadTexture(
    Resource* resource, sf::IntRect inputRect, sf::Vector2u outputSize, bool isRepeating) {
    const bool exists{std::filesystem::exists(resource->path)};
    if(!exists) {
        std::string failMsg{"Failed to load texture file: " + std::string{path} + "."};
        return Error{Error::Signal::InvalidPath, failMsg};
    }
    if(isRepeating) {
        return LoadRepeatingTexture(resource, inputRect, outputSize);
        //return LoadRepeatingTexture(id, path, inputRect, outputSize);
    } // if repeating
    else {
        const auto& textureResult{textures.emplace(resource->id, sf::Texture{resource->path, false, inputRect})};
        if(!textureResult.second) {
            std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
            return Error{Error::Signal::InvalidConfiguration, failMsg};
        } // if loading texture failed
    } // else not repeating
    return std::nullopt;
}
*/
/*
std::optional<farcical::Error> farcical::ResourceManager::LoadRepeatingTexture(
    Resource* resource, sf::IntRect inputRect, sf::Vector2u outputSize) {
    auto createTextureResult{
        textures.emplace(id, sf::Texture{outputSize, false})
    };
    if(createTextureResult.second) {
        // Retrieve the (blank) bigTexture we just created
        sf::Texture* bigTexture{GetTexture(id).value()};

        // Load the smallTexture from disk
        sf::Texture smallTexture{path, false, inputRect};

        // Init fullTexture equal to smallTexture
        sf::Image fullTexture{path};

        // Calculate # of tiles that fit in bigTexture, then copy smallTexture into bigTexture that many times
        const unsigned int widthInTiles{outputSize.x / smallTexture.getSize().x};
        const unsigned int heightInTiles{outputSize.y / smallTexture.getSize().y};
        for(unsigned int y = 0; y < heightInTiles; y++) {
            for(unsigned int x = 0; x < widthInTiles; x++) {
                bigTexture->update(smallTexture, sf::Vector2u{
                                       x * smallTexture.getSize().x,
                                       y * smallTexture.getSize().y});
            } // for x
        } // for y

        // If we have a fractional tile on the x-axis...
        if(smallTexture.getSize().x * widthInTiles < bigTexture->getSize().x) {
            // Calculate the number of remaining pixels
            const sf::Vector2u horizontalSliceSize{
                bigTexture->getSize().x - (smallTexture.getSize().x * widthInTiles),
                smallTexture.getSize().y
            };
            // Grab a "slice" of the fullTexture image with the above size
            sf::Image horizontalSlice{horizontalSliceSize};
            const sf::IntRect sourceRect{
                    {0, 0},
                    {static_cast<int>(horizontalSliceSize.x), static_cast<int>(horizontalSliceSize.y)},
            };
            bool copySuccess{horizontalSlice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
            if(copySuccess) {
                // Copy that slice over at the end of each row
                const sf::Texture hSliceTexture{horizontalSlice};
                for(unsigned int y = 0; y < heightInTiles; y++) {
                    const sf::Vector2u dest{
                        smallTexture.getSize().x * widthInTiles,
                        y * smallTexture.getSize().y
                    };
                    bigTexture->update(hSliceTexture, dest);
                } // for each row
            } // if copySuccess
        } // if smallTexture.width * widthInTiles < bigTexture->width

        // If we have a fractional tile on the y-axis...
        if(smallTexture.getSize().y * heightInTiles < bigTexture->getSize().y) {
            // Calculate the number of remaining pixels
            const sf::Vector2u verticalSliceSize{
                smallTexture.getSize().x,
                bigTexture->getSize().y - (smallTexture.getSize().y * heightInTiles)
            };
            // Grab a "slice" of the fullTexture image with the above size
            sf::Image verticalSlice{verticalSliceSize};
            const sf::IntRect sourceRect{
                    {0, 0},
                    {static_cast<int>(verticalSliceSize.x), static_cast<int>(verticalSliceSize.y)},
                };
            bool copySuccess{verticalSlice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
            if(copySuccess) {
                // Copy that slice over at the end of each column
                const sf::Texture vSliceTexture{verticalSlice};
                for(unsigned int x = 0; x < widthInTiles; x++) {
                    const sf::Vector2u dest{
                        x * smallTexture.getSize().x,
                        smallTexture.getSize().y * heightInTiles,
                    };
                    bigTexture->update(vSliceTexture, dest);
                } // for each column
            } // if copySuccess
        } // if smallTexture.height * heightInTiles < bigTexture->height

        // If we have a fractional tile on both the x-axis AND the y-axis...
        if(smallTexture.getSize().x * widthInTiles < bigTexture->getSize().x
           && smallTexture.getSize().y * heightInTiles < bigTexture->getSize().y) {
            // Calculate the number of remaining pixels
            const sf::Vector2u sliceSize{
                bigTexture->getSize().x - (smallTexture.getSize().x * widthInTiles),
                bigTexture->getSize().y - (smallTexture.getSize().y * heightInTiles)
            };
            // Grab a "slice" of the fullTexture image with the above size
            sf::Image slice{sliceSize};
            const sf::IntRect sourceRect{
                    {0, 0},
                    {static_cast<int>(sliceSize.x), static_cast<int>(sliceSize.y)},
                };
            bool copySuccess{slice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
            if(copySuccess) {
                // Copy that slice over at the end of the last column/row
                const sf::Texture sliceTexture{slice};
                const sf::Vector2u dest{
                    smallTexture.getSize().x * widthInTiles,
                    smallTexture.getSize().y * heightInTiles,
                };
                bigTexture->update(sliceTexture, dest);
            } // if copySuccess
           }
    } // if createTextureResult != Error
    return std::nullopt;
}
*/

/*
std::expected<farcical::ApplicationConfig, farcical::Error> farcical::ResourceManager::LoadAppConfig(std::ifstream& input) {
    this->appConfig.data = nlohmann::json::parse(input);
    ApplicationConfig config{this->appConfig};
    const auto& configJSON{config.data};
    for(auto& [key, value] : configJSON.items()) {
        if(key == "window_properties") {
            for(auto& [winCfgKey, winCfgValue] : value.items()) {
                if(winCfgKey == "width") {
                    config.windowProperties.displayMode.x = winCfgValue.get<int>();
                }
                else if(winCfgKey == "height") {
                    config.windowProperties.displayMode.y = winCfgValue.get<int>();
                }
                else if(winCfgKey == "title") {
                    config.windowProperties.title = winCfgValue.get<std::string>();
                }
                else if(winCfgKey == "fullscreen") {
                    config.windowProperties.fullscreen = winCfgValue.get<bool>();
                }
                else if(winCfgKey == "position") {
                    auto xIter{winCfgValue.find("x")};
                    auto yIter{winCfgValue.find("y")};
                    if(xIter != winCfgValue.end()) {
                        config.windowProperties.position.x = *xIter;
                    }
                    if(yIter != winCfgValue.end()) {
                        config.windowProperties.position.y = *yIter;
                    }
                }
            } // for each key-value pair in windowProperties
        } // if key == window_properties
    } // for each key-value pair in configJSON.items()
    return config;
}

std::expected<farcical::UIConfig, farcical::Error> farcical::ResourceManager::LoadGlobalUIConfig(std::ifstream& input) {
    this->appConfig.data = nlohmann::json::parse(input);
    UIConfig config{this->globalUIConfig};
    const auto& configJSON{config.data};
    for(auto& [key, value] : configJSON.items()) {
        if(key == "menu") {
            for(auto& [menuKey, menuValue]: value.items()) {
                if(menuKey == "buttonSpacing") {
                    config.buttonSpacing = menuValue.get<float>();
                } // if menuKey == buttonSpacing
                else if(menuKey == "title") {
                    for(auto& [titleKey, titleValue]: menuValue.items()) {
                        if(titleKey == "font") {
                            for(auto& [fontKey, fontValue]: titleValue.items()) {
                                if(fontKey == "id") {
                                    config.titleProperties.id = fontValue.get<std::string>();
                                } // if fontKey == id
                                else if(fontKey == "path") {
                                    config.titleProperties.path = fontValue.get<std::string>();
                                } // else if fontKey == path
                                else if(fontKey == "defaultSize") {
                                    config.titleProperties.characterSize = fontValue.get<int>();
                                } // else if fontKey == defaultSize
                                else if(fontKey == "defaultColor") {
                                    const std::string colorName = fontValue.get<std::string>();
                                    const sf::Color color{GetColorByName(colorName)};
                                    config.titleProperties.color = color;
                                } // else if fontKey == defaultColor
                                else if(fontKey == "defaultOutlineColor") {
                                    const std::string colorName = fontValue.get<std::string>();
                                    const sf::Color color{GetColorByName(colorName)};
                                    config.titleProperties.outlineColor = color;
                                } // else if fontKey == defaultOutlineColor
                                else if(fontKey == "defaultOutlineThickness") {
                                    config.titleProperties.outlineThickness = fontValue.get<float>();
                                } // else if fontKey == defaultOutlineThickness
                            } // for each key-value pair in font
                        } // if titleKey == font
                    } // for each key-value pair in title
                } // if menuKey == title
            } // for each key-value pair in menu
        } // if(key == menu)
        else if(key == "button") {
            for(auto& [buttonKey, buttonValue]: value.items()) {
                if(buttonKey == "font") {

                } // if buttonKey == "font"
            } // for each key-value pair in button
        } // else if(key == button)
    } // for each key-value pair in configJSON.items()
    return config;
}

std::expected<farcical::SceneConfig, farcical::Error> farcical::ResourceManager::LoadSceneConfig(std::ifstream& input) {
    this->currentSceneConfig.data = nlohmann::parse(input);
    SceneConfig config{this->currentSceneConfig};
    const auto& configJSON{config.data};
    // CHECK FOR "scene" AT TOP LEVEL
    if(!configJSON.contains("scene")) {
        std::string failMsg{"Invalid configuration: Scene data expected, but could not be found."};
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    } // if !sceneJSON.contains("scene")
    const auto& sceneJSON{configJSON.at("scene")};

    // SCENE MUST HAVE AN ID
    if(!sceneJSON.contains("id")) {
        std::string failMsg{"Invalid configuration: SceneID could not be found."};
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    } // // if !sceneJSON.contains("id")
    config.sceneID = sceneJSON.at("id").get<std::string>();

    // CREATE DECORATIONS (if any)
    if(sceneJSON.contains("decorations")) {
        const auto& decorationsJSON{sceneJSON.at("decorations")};
        const auto& decorationsError{LoadDecorations(decorationsJSON)};
        if(decorationsError.has_value()) {
            return decorationsError;
        }
    } // if sceneJSON.contains("decorations")

    // CREATE TITLE (if any)
    if(sceneJSON.contains("title")) {
        const auto& titleJSON{sceneJSON.at("title")};
        const auto& titleError{LoadTitle(titleJSON, resourceManager)};
        if(titleError.has_value()) {
            return titleError;
        } // if(titleError)
    } // if sceneJSON.contains("title")

    // CREATE MENU (if any)
    if(sceneJSON.contains("menu")) {
        const auto& menuJSON{sceneJSON.at("menu")};
        const auto& menuError{LoadMenu(menuJSON, resourceManager)};
        if(menuError.has_value()) {
            return menuError;
        } // if(menuError)
    } // if sceneJSON.contains("menu")
    return config;
}

std::optional<farcical::Error> farcical::ResourceManager::LoadConfigDecorations(const nlohmann::json& json, SceneConfig& config) {
    for(const auto& decDescription: json) {
        const std::string decorationID{decDescription.at("id").get<std::string>()};

        const auto& textureJSON{decDescription.at("texture")};
        const auto loadTextureResult{LoadConfigTexture(textureJSON)};
        if(!loadTextureResult.has_value()) {
            return loadTextureResult.error();
        } // if loadTextureResult == Error

        const auto& positionJSON{decDescription.at("position")};
        const auto loadPositionResult{LoadPosition(positionJSON)};
        if(!loadPositionResult.has_value()) {
            return loadPositionResult.error();
        } // if loadPositionResult == Error

        const auto& layoutJSON{decDescription.at("layout")};
        const auto loadLayoutResult{LoadLayout(layoutJSON)};
        if(!loadLayoutResult.has_value()) {
            return loadLayoutResult.error();
        } // if loadLayoutResult == Error
        this->layouts.insert(std::make_pair(decorationID, loadLayoutResult.value()));

        const auto createDecResult{Decoration::Create(decorationID, loadTextureResult.value(), this)};
        if(!createDecResult.has_value()) {
            return createDecResult.error();
        }

        Decoration* decoration{createDecResult.value()};
        decoration->SetPosition(loadPositionResult.value());
        ApplyLayout(loadLayoutResult.value(), createDecResult.value());
    } // for each decoration in json
    return std::nullopt;
}

std::optional<farcical::Error> farcical::ResourceManager::LoadConfigTitle(const nlohmann::json& json, SceneConfig& config) {
}

std::optional<farcical::Error> farcical::ResourceManager::LoadConfigMenu(const nlohmann::json& json, SceneConfig& config) {
}

std::expected<sf::Texture*, farcical::Error> farcical::ResourceManager::LoadConfigTexture(const nlohmann::json& json) {
    if(!json.contains("id")) {
        const std::string failMsg{"Invalid configuration: TextureID could not be found."};
        return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
    }
    if(!json.contains("path")) {
        const std::string failMsg{"Invalid configuration: Texture path could not be found."};
        return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
    }
    const std::string id{json.at("id").get<std::string>()};
    const std::string path{json.at("path").get<std::string>()};
    const bool isRepeated{json.at("isRepeated").get<bool>()};
    float scale{1.0f};
    if(json.contains("scale")) {
        scale = json.at("scale").get<float>();
    }
    sf::Vector2u size {0, 0};
    if(json.contains("size")) {
        const auto& sizeJSON{json.at("size")};
        const auto loadSizeResult{LoadConfigSize(sizeJSON)};
        if(!loadSizeResult.has_value()) {
            return std::unexpected(loadSizeResult.error());
        }
        size = loadSizeResult.value();
    }
    const auto result{
        resourceManager.LoadResource(
            id,
            Resource::Type::Texture,
            path,
            sf::Rect{sf::Vector2i{0, 0}, sf::Vector2i{static_cast<int>(size.x), static_cast<int>(size.y)}},
            size,
            isRepeated)
    };
    if(result.has_value()) {
        return std::unexpected(result.value());
    }
    return resourceManager.GetTexture(id);
}

std::expected<sf::Vector2f, farcical::Error> farcical::ResourceManager::LoadConfigPosition(const nlohmann::json& json) {
}

std::expected<sf::Vector2u, farcical::Error> farcical::ResourceManager::LoadConfigSize(const nlohmann::json& json) {
    if(!(json.contains("x")  && json.contains("y"))) {
        const std::string failMsg{"Invalid configuration: Size not valid."};
        return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
    }
    sf::Vector2u size{
        static_cast<unsigned int>(json.at("x").get<int>()),
        static_cast<unsigned int>(json.at("y").get<int>())
    };
    return size;
}

std::expected<farcical::ui::Layout, farcical::Error> farcical::ResourceManager::LoadConfigLayout(const nlohmann::json& json) {
}
*/
/*
std::optional<farcical::Error> farcical::ResourceManager::LoadConfig(ResourceID id, std::string_view path) {
    std::ifstream configFile{std::string{path}, std::ios_base::in};
    if(!configFile.good()) {
        std::string failMsg{"Failed to load config file from " + std::string{path} + "."};
        return Error{Error::Signal::InvalidPath, failMsg};
    }
    Config config;
    config.data = nlohmann::json::parse(configFile);
    if(configFile.is_open()) {
        configFile.close();
    }
    Resource resource{
        .id = id,
        .status = Resource::Status::Uninitialized,
        .type = Resource::Type::Config,
        .path = std::string{path}
    };
    auto registerResult{registry.emplace(id, std::move(resource))};
    if(!registerResult.second) {
        std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    }
    auto configResult{configs.emplace(id, std::move(config))};
    if(!configResult.second) {
        std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    }
    return std::nullopt;
}
*/
/*
std::optional<farcical::Error> farcical::ResourceManager::LoadFont(ResourceID id, std::string_view path) {
    const bool exists{std::filesystem::exists(path)};
    if(!exists) {
        std::string failMsg{"Failed to load font file: " + std::string{path} + "."};
        return Error{Error::Signal::InvalidPath, failMsg};
    }
    auto fontResult{fonts.emplace(id, sf::Font{path})};
    if(!fontResult.second) {
        std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    }
    return std::nullopt;
}
*/
/*
std::optional<farcical::Error> farcical::ResourceManager::LoadTexture(ResourceID id,
                                                                      std::string_view path,
                                                                      sf::IntRect inputRect,
                                                                      sf::Vector2u outputSize,
                                                                      bool repeat) {
    const bool exists{std::filesystem::exists(path)};
    if(!exists) {
        std::string failMsg{"Failed to load texture file: " + std::string{path} + "."};
        return Error{Error::Signal::InvalidPath, failMsg};
    }
    Resource resource{
        .id = id,
        .status = Resource::Status::Uninitialized,
        .type = Resource::Type::Texture,
        .path = std::string{path},
        .rect = inputRect
    };
    auto registerResult{registry.emplace(id, std::move(resource))};
    if(!registerResult.second) {
        std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    }
    if(repeat) {
        return LoadRepeatingTexture(id, path, inputRect, outputSize);
    } // if repeating
    else {
        auto textureResult{textures.emplace(id, sf::Texture{path, false, inputRect})};
        if(!textureResult.second) {
            std::string failMsg{"Invalid configuration (file: " + std::string{path} + "."};
            return Error{Error::Signal::InvalidConfiguration, failMsg};
        } // if loading texture failed
    } // else not repeating
    return std::nullopt;
}
*/
/*
std::optional<farcical::Error> farcical::ResourceManager::LoadRepeatingTexture(
    ResourceID id, std::string_view path, sf::IntRect inputRect, sf::Vector2u outputSize) {
    auto createTextureResult{
        // This texture is henceforth referred to as "bigTexture".
        // We're creating it as a blank canvas here, then filling it with repeating tiles
        // (of "smallTexture") below.
        textures.emplace(id, sf::Texture{outputSize, false})
    };
    if(createTextureResult.second) {
        // Retrieve the (blank) bigTexture we just created
        sf::Texture* bigTexture{GetTexture(id).value()};
        // Load the smallTexture from disk
        sf::Texture smallTexture{path, false, inputRect};
        // Init fullTexture equal to smallTexture
        sf::Image fullTexture{path};

        // Calculate how many tiles will fit in bigTexture, then copy smallTexture into bigTexture
        // that number of times.
        unsigned int widthInTiles{outputSize.x / smallTexture.getSize().x};
        unsigned int heightInTiles{outputSize.y / smallTexture.getSize().y};
        for(unsigned int y = 0; y < heightInTiles; y++) {
            for(unsigned int x = 0; x < widthInTiles; x++) {
                bigTexture->update(smallTexture, sf::Vector2u{
                                       x * smallTexture.getSize().x,
                                       y * smallTexture.getSize().y
                                   });
            } // for x
        } // for y

        // If we have a fractional tile on the x-axis...
        if(smallTexture.getSize().x * widthInTiles < bigTexture->getSize().x) {
            // Calculate the number of remaining pixels
            sf::Vector2u horizontalSliceSize{
                bigTexture->getSize().x - (smallTexture.getSize().x * widthInTiles),
                smallTexture.getSize().y
            };
            // Grab a "slice" of the fullTexture image with the above size
            sf::Image horizontalSlice{horizontalSliceSize};
            sf::IntRect sourceRect{
                {0, 0},
                {static_cast<int>(horizontalSliceSize.x), static_cast<int>(horizontalSliceSize.y)},
            };
            bool copySuccess{horizontalSlice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
            if(copySuccess) {
                // Copy that slice over at the end of each row
                sf::Texture hSliceTexture{horizontalSlice};
                for(unsigned int y = 0; y < heightInTiles; y++) {
                    sf::Vector2u dest{
                        smallTexture.getSize().x * widthInTiles,
                        y * smallTexture.getSize().y
                    };
                    bigTexture->update(hSliceTexture, dest);
                } // for each row
            } // if copySuccess
        } // if smallTexture.width * widthInTiles < bigTexture->width

        // If we have a fractional tile on the y-axis...
        if(smallTexture.getSize().y * heightInTiles < bigTexture->getSize().y) {
            // Calculate the number of remaining pixels
            sf::Vector2u verticalSliceSize{
                smallTexture.getSize().x,
                bigTexture->getSize().y - (smallTexture.getSize().y * heightInTiles)
            };
            // Grab a "slice" of the fullTexture image with the above size
            sf::Image verticalSlice{verticalSliceSize};
            sf::IntRect sourceRect{
                {0, 0},
                {static_cast<int>(verticalSliceSize.x), static_cast<int>(verticalSliceSize.y)},
            };
            bool copySuccess{verticalSlice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
            if(copySuccess) {
                // Copy that slice over at the end of each column
                sf::Texture vSliceTexture{verticalSlice};
                for(unsigned int x = 0; x < widthInTiles; x++) {
                    sf::Vector2u dest{
                        x * smallTexture.getSize().x,
                        smallTexture.getSize().y * heightInTiles,
                    };
                    bigTexture->update(vSliceTexture, dest);
                } // for each column
            } // if copySuccess
        } // if smallTexture.height * heightInTiles < bigTexture->height

        // If we have a fractional tile on both the x-axis AND the y-axis...
        if(smallTexture.getSize().x * widthInTiles < bigTexture->getSize().x
           && smallTexture.getSize().y * heightInTiles < bigTexture->getSize().y) {
            // Calculate the number of remaining pixels
            sf::Vector2u sliceSize{
                bigTexture->getSize().x - (smallTexture.getSize().x * widthInTiles),
                bigTexture->getSize().y - (smallTexture.getSize().y * heightInTiles)
            };
            // Grab a "slice" of the fullTexture image with the above size
            sf::Image slice{sliceSize};
            sf::IntRect sourceRect{
                {0, 0},
                {static_cast<int>(sliceSize.x), static_cast<int>(sliceSize.y)},
            };
            bool copySuccess{slice.copy(fullTexture, sf::Vector2u{0, 0}, sourceRect)};
            if(copySuccess) {
                // Copy that slice over at the end of the last column/row
                sf::Texture sliceTexture{slice};
                sf::Vector2u dest{
                    smallTexture.getSize().x * widthInTiles,
                    smallTexture.getSize().y * heightInTiles,
                };
                bigTexture->update(sliceTexture, dest);
            } // if copySuccess
        } /* if smallTexture.width * widthInTiles < bigTexture->width &&
             smallTexture.height * heightInTiles < bigTexture->height */
/*
    } // if texture was successfully created
    return std::nullopt;
}
*/