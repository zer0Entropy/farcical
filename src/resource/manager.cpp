//
// Created by dgmuller on 6/4/25.
//

#include <fstream>
#include <SFML/Graphics/Image.hpp>
#include "../../include/resource/manager.hpp"
#include "../../include/geometry.hpp"

farcical::ResourceManager::ResourceManager():
    logSystem{nullptr} {
}

void farcical::ResourceManager::AddLogSystem(engine::LogSystem* logSystem) {
    this->logSystem = logSystem;
}

farcical::ResourceHandle* farcical::ResourceManager::GetResourceHandle(ResourceID id) const {
    const auto resourceIter{registry.find(id)};
    if(logSystem) {
        logSystem->AddMessage("Resource requested (id=\"" + id + "\").");
    }
    if(resourceIter != registry.end()) {
        const ResourceHandle* handle{&resourceIter->second};
        if(logSystem) {
            logSystem->AddMessage("Resource with id=\"" + id + "\" found (type=" + std::string{ResourceHandle::GetTypeName(handle->type)} + ").");
        }
        return const_cast<ResourceHandle*>(handle);
    }
    if(logSystem) {
        logSystem->AddMessage("Failed to find Resource with id=\"" + id + "\"!");
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
    if(logSystem) {
        logSystem->AddMessage("ResourceHandle (id=\"" + id + "\", type=" + std::string{ResourceHandle::GetTypeName(type)} + ") created.");
    }
    return &createResourceResult.first->second;
}

std::optional<farcical::engine::Error> farcical::ResourceManager::DestroyResourceHandle(ResourceID id, ResourceHandle::Type type) {
    switch(type) {
        case ResourceHandle::Type::Log: {
            const auto& findLog{logs.find(id)};
            if(findLog != logs.end()) {
                logs.erase(findLog);
                logSystem->AddMessage("Log with id=\"" + id + "\" destroyed.");
            }
        }
        break;
        case ResourceHandle::Type::JSONDocument: {
            const auto& findJSONDoc{jsonDocs.find(id)};
            if(findJSONDoc != jsonDocs.end()) {
                jsonDocs.erase(findJSONDoc);
                logSystem->AddMessage("jsonDocument with id=\"" + id + "\" destroyed.");
            }
        } break;
        case ResourceHandle::Type::Font: {
            const auto& findFont{fonts.find(id)};
            if(findFont != fonts.end()) {
                fonts.erase(findFont);
                logSystem->AddMessage("Font with id=\"" + id + "\" destroyed.");
            }
        } break;
        case ResourceHandle::Type::Texture: {
            const auto& findTexture{textures.find(id)};
            if(findTexture != textures.end()) {
                textures.erase(findTexture);
                logSystem->AddMessage("Texture with id=\"" + id + "\" destroyed.");
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

std::expected<farcical::engine::Log*, farcical::engine::Error> farcical::ResourceManager::GetLog(ResourceID id) {
    ResourceHandle* handle{GetResourceHandle(id)};
    // If a ResourceHandle with this ResourceID has not been created previously, return Error{ResourceNotFound}
    if(!handle) {
        const std::string failMsg{"Resource not found: " + id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if handle does not exist
    if(handle->status == ResourceHandle::Status::IsReady) {
        // Confirm this Log has already been created
        const auto& logIter{logs.find(id)};
        if(logIter != logs.end()) {
            return &logIter->second;
        } // if Log found
        // If it wasn't found, mark the ResourceHandle with an Error status & return Error{ResourceNotFound}
        handle->status = ResourceHandle::Status::Error;
        const std::string failMsg{"Resource not found: " + id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if ResourceHandle is marked IsReady
    if(handle->status == ResourceHandle::Status::Uninitialized) {
        // Create the Log and load it from file
        const auto& createLog{logs.emplace(handle->id, engine::Log{{handle->id, handle->path}, {}})};
        const auto& logIter{createLog.first};
        std::ifstream inputFromFile{handle->path};
        logSystem->AddMessage("Attempting to open Log (id=\"" + id + "\" at " + handle->path + "...");
        if(!std::filesystem::exists(handle->path)) {
            logSystem->AddMessage("File not found, creating new Log file at " + handle->path + "...");
            std::ofstream outputToFile{handle->path, std::ios_base::out};
            outputToFile.close();
            inputFromFile.open(handle->path);
            logSystem->AddMessage("Log file successfully created at " + handle->path + ".");
        }
        while(inputFromFile.good()) {
            std::string nextLine{""};
            std::getline(inputFromFile, nextLine);
            if(!nextLine.empty()) {
                logIter->second.contents.push_back(nextLine);
            } // if !nextLine.empty()
        } // if inputFromFile.good()
        // If the file was successfully opened, close it
        if(inputFromFile.is_open()) {
            if(!logIter->second.contents.empty()) {
                logSystem->AddMessage("Log file successfully loaded from " + handle->path + ".");
            } // if log is not empty
            inputFromFile.close();
            handle->status = ResourceHandle::Status::IsReady;
            return &logIter->second;
        } // if inputFromFile.is_open()
        // Otherwise, return Error{InvalidPath}
        const std::string failMsg{"Invalid path: Could not open Log at " + handle->path + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
    } // if ResourceHandle is marked Uninitialized
    // If we reach this code path, an Error has occurred!
    const std::string failMsg{"Resource not found: " + id + "."};
    return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
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
        // If it wasn't found, mark the ResourceHandle with an Error status & return Error{ResourceNotFound}
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

    if(handle->status == ResourceHandle::Status::Uninitialized) {
        // Create the Font and load it from file
        const auto& fontResult{fonts.emplace(id, sf::Font{handle->path})};
        if(!fontResult.second) {
            const std::string failMsg{"Invalid path: Could not open Font at " + handle->path + "."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
        } // if fontResult.second == false
        handle->status = ResourceHandle::Status::IsReady;
        return &fontResult.first->second;
    } // if ResourceHandle is marked Uninitialized

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
    } // if ResourceHandle is marked Uninitialized

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
        const auto& textureResult{textures.emplace(properties.id, sf::Texture{handle->path, false})};
        if(!textureResult.second) {
            const std::string failMsg{"Invalid path: Could not open Texture at " + handle->path + "."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidPath, failMsg});
        } // if textureResult.second == false
        handle->status = ResourceHandle::Status::IsReady;
        return &textureResult.first->second;
    } // if ResourceHandle is marked Uninitialized

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

std::optional<farcical::engine::Error> farcical::ResourceManager::WriteLog(
    ResourceID id, const std::vector<std::string>& messages) {
    ResourceHandle* handle{GetResourceHandle(id)};
    if(!handle) {
        const std::string failMsg{"Resource not found: " + id + "."};
        return engine::Error{engine::Error::Signal::ResourceNotFound, failMsg};
    } // if ResourceHandle not found
    std::ofstream outputFile{handle->path, std::ios_base::out};
    if(outputFile.good()) {
        for(const auto& message: messages) {
            outputFile << message << std::endl;
        } // for each message
    } // if outputFile.good()
    if(outputFile.is_open()) {
        outputFile.close();
    }
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ResourceManager::AppendToLog(ResourceID id, const std::vector<std::string>& messages) {
    ResourceHandle* handle{GetResourceHandle(id)};
    if(!handle) {
        const std::string failMsg{"Resource not found: " + id + "."};
        return engine::Error{engine::Error::Signal::ResourceNotFound, failMsg};
    } // if ResourceHandle not found
    std::ofstream outputFile{handle->path, std::ios_base::out | std::ios_base::app};
    if(outputFile.good()) {
        for(const auto& message: messages) {
            outputFile << message << std::endl;
        } // for each message
    } // if outputFile.good()
    if(outputFile.is_open()) {
        outputFile.close();
    }
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ResourceManager::AppendToLog(engine::Log* log, const std::vector<std::string>& messages) {
    if(!log) {
        const std::string failMsg{"Unexpected nullptr: log"};
        return engine::Error{engine::Error::Signal::NullPtr, failMsg};
    } // if !log
    std::ofstream outputFile{log->resource.second, std::ios_base::out | std::ios_base::app};
    if(outputFile.good()) {
        for(const auto& message: messages) {
            outputFile << message << std::endl;
        } // for each message
    } // if outputFile.good()
    if(outputFile.is_open()) {
        outputFile.close();
    }
    return std::nullopt;
}

std::expected<sf::Texture*, farcical::engine::Error> farcical::ResourceManager::CreateSplicedTexture(
    ResourceID id, const std::vector<ResourceID>& inputTextureIDs) {
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
    ResourceID id, const sf::Vector2u& outputSize, ResourceID inputID, sf::IntRect inputRect) {
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

std::expected<sf::Texture*, farcical::engine::Error> farcical::ResourceManager::CreateBorderTexture(
    ResourceID id,
    const sf::Vector2u& outputSize,
    const std::vector<ResourceID>& cornerTextureIDs,
    const std::vector<ResourceID>& edgeTextureIDs,
    ResourceID centerTextureID) {
    ResourceHandle* borderHandle{GetResourceHandle(id)};
    // If a ResourceHandle with this ID has not been created previously, create it
    if(!borderHandle) {
        const auto& createBorderHandle{CreateResourceHandle(id, ResourceHandle::Type::Texture, "")};
        if(!createBorderHandle.has_value()) {
            const std::string failMsg{"Failed to create ResourceHandle for Border (id=" + id + "\")."};
            return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
        } // if createBorderHadle failed, return Error
        borderHandle = createBorderHandle.value();
    } // if handle does not exist

    // Ensure all cornerTextures and edgeTextures have valid handles and are ready to use
    for(const auto& cornerTextureID: cornerTextureIDs) {
        ResourceHandle* cornerHandle{GetResourceHandle(cornerTextureID)};
        if(!cornerHandle || cornerHandle->status != ResourceHandle::Status::IsReady) {
            const std::string failMsg{"Resource not found: Texture (id=" + cornerTextureID + "\")."};
            return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
        } // if !cornerHandle or cornerHandle->status != IsReady
    } // for each cornerTextureID
    for(const auto& edgeTextureID: edgeTextureIDs) {
        ResourceHandle* edgeHandle{GetResourceHandle(edgeTextureID)};
        if(!edgeHandle || edgeHandle->status != ResourceHandle::Status::IsReady) {
            const std::string failMsg{"Resource not found: Texture (id=" + edgeTextureID + "\")."};
            return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
        } // if !edgeHandle or edgeHandle->status != IsReady
    } // for each edgeTextureID
    // Check the center, too
    ResourceHandle* centerHandle{GetResourceHandle(centerTextureID)};
    if(!centerHandle || centerHandle->status != ResourceHandle::Status::IsReady) {
        const std::string failMsg{"Resource not found: Texture (id=" + centerTextureID + "\")."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if !centerHandle or centerHandle->status != IsReady

    // Prepare our input Textures
    std::array<sf::Texture*, static_cast<int>(Rectangle::Corner::NumCorners)> cornerTextures;
    for(int index = static_cast<int>(Rectangle::Corner::TopLeft);
        index < static_cast<int>(Rectangle::Corner::NumCorners); ++index) {
        const auto& requestTexture{GetTexture(cornerTextureIDs[index])};
        if(!requestTexture.has_value()) {
            const std::string failMsg{"Resource not found: Texture (id=" + cornerTextureIDs[index] + "\")."};
            return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
        } // if requestTexture failed
        cornerTextures[index] = requestTexture.value();
    } // for each Corner

    std::array<sf::Texture*, static_cast<int>(Rectangle::Edge::NumEdges)> edgeTextures;
    for(int index = static_cast<int>(Rectangle::Edge::Left); index < static_cast<int>(Rectangle::Edge::NumEdges); ++
        index) {
        const auto& requestTexture{GetTexture(edgeTextureIDs[index])};
        if(!requestTexture.has_value()) {
            const std::string failMsg{"Resource not found: Texture (id=" + cornerTextureIDs[index] + "\")."};
            return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
        } // if requestTexture failed
        edgeTextures[index] = requestTexture.value();
    } // for each Edge
    const auto& requestTexture{GetTexture(centerTextureID)};
    if(!requestTexture.has_value()) {
        const std::string failMsg{"Resource not found: Texture (id=" + centerTextureID + "\")."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if requestTexture failed
    sf::Texture* centerTexture{requestTexture.value()};

    // Create a blank canvas for our Border
    const auto& createTextureResult{
        textures.emplace(id, sf::Texture{outputSize, false})
    };
    if(createTextureResult.second) {
        sf::Texture* outputTexture{nullptr};
        // Retrieve the blank Texture we just created
        const auto& findOutputTexture{textures.find(id)};
        if(findOutputTexture == textures.end()) {
            const std::string failMsg{"Resource not found: " + id + "."};
            return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
        } // if outputTexture not found
        outputTexture = &findOutputTexture->second;

        // Copy corners into the outputTexture
        const sf::Vector2u topLeftPosition{0, 0};
        outputTexture->update(*cornerTextures[static_cast<int>(Rectangle::Corner::TopLeft)], topLeftPosition);
        const sf::Vector2u topRightPosition{
            outputSize.x - cornerTextures[static_cast<int>(Rectangle::Corner::TopRight)]->getSize().x, 0
        };
        outputTexture->update(*cornerTextures[static_cast<int>(Rectangle::Corner::TopRight)], topRightPosition);
        const sf::Vector2u bottomLeftPosition{
            0, outputSize.y - cornerTextures[static_cast<int>(Rectangle::Corner::BottomLeft)]->getSize().y
        };
        outputTexture->update(*cornerTextures[static_cast<int>(Rectangle::Corner::BottomLeft)], bottomLeftPosition);
        const sf::Vector2u bottomRightPosition{
            outputSize.x - cornerTextures[static_cast<int>(Rectangle::Corner::TopRight)]->getSize().x,
            outputSize.y - cornerTextures[static_cast<int>(Rectangle::Corner::BottomLeft)]->getSize().y
        };
        outputTexture->update(*cornerTextures[static_cast<int>(Rectangle::Corner::BottomRight)], bottomRightPosition);

        // Create edges of the appropriate size by repeating our input edge Textures
        // Vertical edges - Left and Right
        const sf::Vector2u vEdgeSize{
            edgeTextures[static_cast<int>(Rectangle::Edge::Left)]->getSize().x,
            outputSize.y
            - cornerTextures[static_cast<int>(Rectangle::Corner::TopLeft)]->getSize().y
            - cornerTextures[static_cast<int>(Rectangle::Corner::BottomLeft)]->getSize().y
        };
        // Horizontal edges - Top and Bottom
        const sf::Vector2u hEdgeSize{
            outputSize.x
            - cornerTextures[static_cast<int>(Rectangle::Corner::TopLeft)]->getSize().x
            - cornerTextures[static_cast<int>(Rectangle::Corner::TopRight)]->getSize().x,
            edgeTextures[static_cast<int>(Rectangle::Edge::Top)]->getSize().y
        };
        sf::Texture topTexture{hEdgeSize, false};
        sf::Texture bottomTexture{hEdgeSize, false};
        sf::Texture leftTexture{vEdgeSize, false};
        sf::Texture rightTexture{vEdgeSize, false};

        // Create the edges
        RepeatTexture(*edgeTextures[static_cast<int>(Rectangle::Edge::Top)], topTexture);
        RepeatTexture(*edgeTextures[static_cast<int>(Rectangle::Edge::Bottom)], bottomTexture);
        RepeatTexture(*edgeTextures[static_cast<int>(Rectangle::Edge::Left)], leftTexture);
        RepeatTexture(*edgeTextures[static_cast<int>(Rectangle::Edge::Right)], rightTexture);

        // Check to see whether the tiles filled the Texture evenly, or if there are "slices" missing
        // If so, fill in those slices
        unsigned int widthInTiles{hEdgeSize.x / edgeTextures[static_cast<int>(Rectangle::Edge::Top)]->getSize().x};
        unsigned int heightInTiles{vEdgeSize.y / edgeTextures[static_cast<int>(Rectangle::Edge::Left)]->getSize().y};
        if(edgeTextures[static_cast<int>(Rectangle::Edge::Top)]->getSize().x * widthInTiles < hEdgeSize.x) {
            RepeatSliceHorizontal(*edgeTextures[static_cast<int>(Rectangle::Edge::Top)], topTexture);
        } // if edge tile width * widthInTiles < edge width
        if(edgeTextures[static_cast<int>(Rectangle::Edge::Bottom)]->getSize().x * widthInTiles < hEdgeSize.x) {
            RepeatSliceHorizontal(*edgeTextures[static_cast<int>(Rectangle::Edge::Bottom)], bottomTexture);
        } // if edge tile with * widthInTiles < edge width
        if(edgeTextures[static_cast<int>(Rectangle::Edge::Left)]->getSize().y * heightInTiles < vEdgeSize.y) {
            RepeatSliceVertical(*edgeTextures[static_cast<int>(Rectangle::Edge::Left)], leftTexture);
        } // if edge tile height * heightInTiles < edge height
        if(edgeTextures[static_cast<int>(Rectangle::Edge::Top)]->getSize().y * heightInTiles < vEdgeSize.y) {
            RepeatSliceVertical(*edgeTextures[static_cast<int>(Rectangle::Edge::Right)], rightTexture);
        } // if edge tile height * heightInTiles < edge height

        // Copy them into the outputTexture
        const sf::Vector2u topPosition{
            topLeftPosition.x + cornerTextures[static_cast<int>(Rectangle::Corner::TopLeft)]->getSize().x,
            0
        };
        outputTexture->update(topTexture, topPosition);
        const sf::Vector2u bottomPosition{
            bottomLeftPosition.x + cornerTextures[static_cast<int>(Rectangle::Corner::BottomLeft)]->getSize().x,
            outputSize.y - bottomTexture.getSize().y
        };
        outputTexture->update(bottomTexture, bottomPosition);
        const sf::Vector2u leftPosition{
            0,
            topLeftPosition.y + cornerTextures[static_cast<int>(Rectangle::Corner::TopLeft)]->getSize().y
        };
        outputTexture->update(leftTexture, leftPosition);
        const sf::Vector2u rightPosition{
            topRightPosition.x,
            topRightPosition.y + cornerTextures[static_cast<int>(Rectangle::Corner::TopRight)]->getSize().y
        };
        outputTexture->update(rightTexture, rightPosition);

        // Create the center
        sf::Vector2u centerSize{
            outputSize.x - leftTexture.getSize().x - rightTexture.getSize().x,
            outputSize.y - topTexture.getSize().y - bottomTexture.getSize().y
        };
        sf::Texture bigCenterTexture{centerSize, false};
        RepeatTexture(*centerTexture, bigCenterTexture);

        // Check to see whether the tiles filled the Texture evenly, or if there are "slices" missing
        // If so, fill those slices in
        unsigned int centerWidthInTiles{centerSize.x / centerTexture->getSize().x};
        unsigned int centerHeightInTiles{centerSize.y / centerTexture->getSize().y};
        if(centerTexture->getSize().x * centerWidthInTiles < centerSize.x) {
            RepeatSliceHorizontal(*centerTexture, bigCenterTexture);
        } // if center tile width * widthInTiles < center width
        if(centerTexture->getSize().y * centerHeightInTiles < centerSize.y) {
            RepeatSliceVertical(*centerTexture, bigCenterTexture);
        } // if center tile height * heightInTiles < center height

        // Copy it into the outputTexture
        sf::Vector2u centerPosition{
            leftPosition.x + leftTexture.getSize().x,
            leftPosition.y
        };
        outputTexture->update(bigCenterTexture, centerPosition);
        return outputTexture;
    } // if createTextureResult == success

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
