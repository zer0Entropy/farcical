//
// Created by dgmuller on 7/6/25.
//
#include "../../include/resource/config.hpp"

#include "../../include/color.hpp"
#include "../../include/resource/parser.hpp"

std::expected<farcical::FontProperties, farcical::engine::Error> farcical::LoadFontProperties(
    const nlohmann::json& json) {
    FontProperties properties;

    const auto& findID{json.find("id")};
    const auto& findPath{json.find("path")};
    const auto& findScale{json.find("scale")};
    const auto& findSize{json.find("characterSize")};
    const auto& findColor{json.find("color")};
    const auto& findOutlineColor{json.find("outlineColor")};
    const auto& findOutlineThickness{json.find("outlineThickness")};

    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if "id" not found
    properties.id = findID.value().get<std::string>();

    if(findPath == json.end()) {
        const std::string failMsg{"Invalid configuration: Path not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if "path" not found
    properties.path = findPath.value().get<std::string>();

    if(findScale != json.end()) {
        properties.scale = findScale.value().get<float>();
    } // if "scale" found
    if(findSize != json.end()) {
        properties.characterSize = findSize.value().get<int>();
    } // if "characterSize" found
    if(findColor != json.end()) {
        properties.color = GetColorByName(findColor.value().get<std::string>());
    } // if "color" found
    if(findOutlineColor != json.end()) {
        properties.outlineColor = GetColorByName(findOutlineColor.value().get<std::string>());
    } // if "outlineColor" found
    if(findOutlineThickness != json.end()) {
        properties.outlineThickness = findOutlineThickness.value().get<float>();
    } // if "outlineThickness" found

    return properties;
}

std::expected<farcical::TextureProperties, farcical::engine::Error> farcical::LoadTextureProperties(
    const nlohmann::json& json, bool isSegment, std::string_view path) {
    TextureProperties properties;

    const auto& findID{json.find("id")};
    const auto& findPath{json.find("path")};
    const auto& findScale{json.find("scale")};
    const auto& findPosition{json.find("position")};
    const auto& findInputSize{json.find("inputSize")};

    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if "id" not found
    properties.id = findID.value().get<std::string>();

    if(findPath == json.end()) {
        if(isSegment) {
            properties.path = path;
        } // segments inherit the path of their parent texture
        else {
            const std::string failMsg{"Invalid configuration: Path not found."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // all other textures require a path
    } // if "path" not found
    else {
        properties.path = findPath.value().get<std::string>();
    }

    if(findScale != json.end()) {
        properties.scale = findScale.value().get<float>();
    } // if "scale" found
    if(findPosition != json.end()) {
        const auto& positionJSON{findPosition.value()};
        const auto& loadPosition{jsonParser::ParsePosition(positionJSON)};
        if(loadPosition.has_value()) {
            const auto& position{loadPosition.value()};
            properties.inputRect.position = sf::Vector2i{
                static_cast<int>(position.x),
                static_cast<int>(position.y)
            };
        } // if loadPosition == success
    } // if "position" found
    if(findInputSize != json.end()) {
        const auto& loadSize{jsonParser::ParseSize(findInputSize.value())};
        if(loadSize.has_value()) {
            const auto& size{loadSize.value()};
            properties.inputRect.size = sf::Vector2i{
                static_cast<int>(size.x),
                static_cast<int>(size.y)
            };
        } // if loadSize == success
    } // if "inputSize" found

    return properties;
}

std::expected<farcical::RepeatingTextureProperties, farcical::engine::Error> farcical::LoadRepeatingTextureProperties(
    const nlohmann::json& json) {
    RepeatingTextureProperties properties;

    const auto& findInputID{json.find("inputID")};
    const auto& findOutputID{json.find("outputID")};
    const auto& findPath{json.find("path")};
    const auto& findScale{json.find("scale")};
    const auto& findPosition{json.find("position")};
    const auto& findInputSize{json.find("inputSize")};
    const auto& findOutputSize{json.find("outputSize")};

    if(findInputID == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if inputID not found
    properties.inputID = findInputID.value().get<std::string>();

    if(findOutputID == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if outputID not found
    properties.outputID = findOutputID.value().get<std::string>();

    if(findPath == json.end()) {
        const std::string failMsg{"Invalid configuration: Path not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if path not found
    properties.path = findPath.value().get<std::string>();

    if(findOutputSize == json.end()) {
        const std::string failMsg{"Invalid configuration: OutputSize not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    }
    const auto& loadOutputSize{jsonParser::ParseSize(findOutputSize.value())};
    if(loadOutputSize.has_value()) {
        properties.outputSize = loadOutputSize.value();
    } // if loadOutputSize == success

    if(findPosition != json.end()) {
        const auto& loadPosition{jsonParser::ParsePosition(findPosition.value())};
        if(loadPosition.has_value()) {
            properties.inputRect.position = sf::Vector2i{
                static_cast<int>(loadPosition.value().x),
                static_cast<int>(loadPosition.value().y)
            };
        } // if loadPosition == success
    } // if position found

    if(findInputSize != json.end()) {
        const auto& loadSize{jsonParser::ParseSize(findInputSize.value())};
        if(loadSize.has_value()) {
            properties.inputRect.size = sf::Vector2i{
                static_cast<int>(loadSize.value().x),
                static_cast<int>(loadSize.value().y)
            };
        } // if loadSize == success
    } // if inputSize found

    if(findScale != json.end()) {
        properties.scale = findScale.value().get<float>();
    } // if scale found

    return properties;
}

std::expected<farcical::SegmentedTextureProperties, farcical::engine::Error> farcical::LoadSegmentedTextureProperties(
    const nlohmann::json& json) {
    SegmentedTextureProperties properties;

    const auto& findID{json.find("id")};
    const auto& findPath{json.find("path")};
    const auto& findScale{json.find("scale")};
    const auto& findSegments{json.find("segments")};

    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if "id" not found
    properties.id = findID.value().get<std::string>();

    if(findPath == json.end()) {
        const std::string failMsg{"Invalid configuration: Path not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if "path" not found
    properties.path = findPath.value().get<std::string>();

    if(findSegments == json.end()) {
        const std::string failMsg{"Invalid configuration: Segments not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if "segments" not found

    const auto& segmentsJSON{findSegments.value()};
    for(const auto& segmentJSON: segmentsJSON) {
        std::string segmentDump{segmentJSON.dump()};
        const auto& loadSegmentProperties{LoadTextureProperties(segmentJSON, true, properties.path)};
        if(loadSegmentProperties.has_value()) {
            properties.segments.push_back(loadSegmentProperties.value());
        }

        // Find the size of this segment; add its width to parentWidth, and set parentHeight to its height (if greater)
        const auto& findSize{segmentJSON.find("inputSize")};
        if(findSize == segmentJSON.end()) {
            const std::string failMsg{"Invalid configuration: Size not found."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if "size" not found
        const auto& sizeJSON{findSize.value()};
        const auto& loadSizeResult{jsonParser::ParseSize(sizeJSON)};
        if(loadSizeResult.has_value()) {
            const sf::Vector2u size{loadSizeResult.value()};
            properties.outputSize.x += size.x;
            if(properties.outputSize.y < size.y) {
                properties.outputSize.y = size.y;
            } // if outputSize.y < size.y
        } // if loadSizeResult == success
    }

    if(findScale != json.end()) {
        properties.scale = findScale.value().get<float>();
    }

    return properties;
}
