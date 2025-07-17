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

std::expected<farcical::BorderTextureProperties, farcical::engine::Error> farcical::LoadBorderTextureProperties(
    const nlohmann::json& json) {

    BorderTextureProperties properties;
    const auto& findID{json.find("id")};
    const auto& findPath{json.find("path")};
    const auto& findScale{json.find("scale")};
    const auto& findPercentSize{json.find("percentSize")};
    const auto& findCorners{json.find("corners")};
    const auto& findEdges{json.find("edges")};
    const auto& findCenter{json.find("center")};
    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if ResourceID not found
    properties.id = findID.value().get<std::string>();

    if(findPath == json.end()) {
        const std::string failMsg{"Invalid configuration: Path not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if path not found
    properties.path = findPath.value().get<std::string>();

    if(findScale != json.end()) {
        properties.scale = findScale.value().get<float>();
    } // if scale found

    if(findCorners == json.end()) {
        const std::string failMsg{"Invalid configuration: Corners not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if corners not found
    const auto& cornersJSON{findCorners.value()};
    int cornerIndex{0};
    for(const auto& cornerJSON: cornersJSON) {
        const auto& findCornerID{cornerJSON.find("id")};
        const auto& findCornerPosition{cornerJSON.find("position")};
        const auto& findInputSize{cornerJSON.find("inputSize")};

        if(findCornerID == cornerJSON.end()) {
            const std::string failMsg{"Invalid configuration: CornerID not found."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if corners not found
        std::string cornerDump{cornerJSON.dump()};
        properties.cornerTextures[cornerIndex].id = findCornerID.value().get<std::string>();
        properties.cornerTextures[cornerIndex].id += "CornerTexture";

        if(findCornerPosition == cornerJSON.end()) {
            const std::string failMsg{"Invalid configuration: Position not found."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if position not found
        const auto& positionJSON{findCornerPosition.value()};
        const auto& findX{positionJSON.find("x")};
        const auto& findY{positionJSON.find("y")};
        properties.cornerTextures[cornerIndex].inputRect.position.x = findX.value().get<int>();
        properties.cornerTextures[cornerIndex].inputRect.position.y = findY.value().get<int>();

        if(findInputSize == cornerJSON.end()) {
            const std::string failMsg{"Invalid configuration: InputSize not found."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if inputSize not found
        const auto& sizeJSON{findInputSize.value()};
        const auto& findWidth{sizeJSON.find("width")};
        const auto& findHeight{sizeJSON.find("height")};
        properties.cornerTextures[cornerIndex].inputRect.size.x = findWidth.value().get<int>();
        properties.cornerTextures[cornerIndex].inputRect.size.y = findHeight.value().get<int>();
        properties.cornerTextures[cornerIndex].path = properties.path;
        properties.cornerTextures[cornerIndex].scale = properties.scale;
        ++cornerIndex;
    } // for each corner in cornersJSON

    if(findEdges == json.end()) {
        const std::string failMsg{"Invalid configuration: Edges not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if edges not found
    int edgeIndex{0};
    const auto& edgesJSON{findEdges.value()};
    for(const auto& edgeJSON: edgesJSON) {
        const auto& findEdgeID{edgeJSON.find("id")};
        const auto& findEdgePosition{edgeJSON.find("position")};
        const auto& findInputSize{edgeJSON.find("inputSize")};

        if(findEdgeID == edgeJSON.end()) {
            const std::string failMsg{"Invalid configuration: EdgeID not found."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if corners not found
        properties.edgeTextures[edgeIndex].id = findEdgeID.value().get<std::string>();
        properties.edgeTextures[edgeIndex].id += "EdgeTexture";

        if(findEdgePosition == edgeJSON.end()) {
            const std::string failMsg{"Invalid configuration: Position not found."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if position not found
        const auto& positionJSON{findEdgePosition.value()};
        const auto& findX{positionJSON.find("x")};
        const auto& findY{positionJSON.find("y")};
        properties.edgeTextures[edgeIndex].inputRect.position.x = findX.value().get<int>();
        properties.edgeTextures[edgeIndex].inputRect.position.y = findY.value().get<int>();

        if(findInputSize == edgeJSON.end()) {
            const std::string failMsg{"Invalid configuration: InputSize not found."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if position not found
        const auto& sizeJSON{findInputSize.value()};
        const auto& findWidth{sizeJSON.find("width")};
        const auto& findHeight{sizeJSON.find("height")};
        properties.edgeTextures[edgeIndex].inputRect.size.x = findWidth.value().get<int>();
        properties.edgeTextures[edgeIndex].inputRect.size.y = findHeight.value().get<int>();
        properties.edgeTextures[edgeIndex].path = properties.path;
        properties.edgeTextures[edgeIndex].scale = properties.scale;
        edgeIndex++;
    } // for each edge in edgesJSON

    if(findCenter == json.end()) {
        const std::string failMsg{"Invalid configuration: Center not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if center not found
    const auto& centerJSON{findCenter.value()};
    const auto& findCenterID{centerJSON.find("id")};
    if(findCenterID == centerJSON.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if ResourceID not found
    properties.centerTexture.id = findCenterID.value().get<std::string>();
    properties.centerTexture.id += "Texture";

    const auto& findPosition{centerJSON.find("position")};
    if(findPosition == centerJSON.end()) {
        const std::string failMsg{"Invalid configuration: Position not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if position not found
    const auto& positionJSON{findPosition.value()};
    const auto& findX{positionJSON.find("x")};
    const auto& findY{positionJSON.find("y")};
    properties.centerTexture.inputRect.position.x = findX.value().get<int>();
    properties.centerTexture.inputRect.position.y = findY.value().get<int>();

    const auto& findInputSize{centerJSON.find("inputSize")};
    if(findInputSize == centerJSON.end()) {
        const std::string failMsg{"Invalid configuration: InputSize not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if inputSize not found
    const auto& inputSizeJSON{findInputSize.value()};
    const auto& findCenterWidth{inputSizeJSON.find("width")};
    const auto& findCenterHeight{inputSizeJSON.find("height")};
    properties.centerTexture.inputRect.size.x = findCenterWidth.value().get<int>();
    properties.centerTexture.inputRect.size.y = findCenterHeight.value().get<int>();

    properties.centerTexture.path = properties.path;
    properties.centerTexture.scale = properties.scale;

    if(findPercentSize == json.end()) {
        const std::string failMsg{"Invalid configuration: percentSize not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if percentSize not found
    const auto& percentSizeJSON{findPercentSize.value()};
    const auto& findWidth{percentSizeJSON.find("width")};
    const auto& findHeight{percentSizeJSON.find("height")};
    properties.percentSize.x = findWidth.value();
    properties.percentSize.y = findHeight.value();

    return properties;
}
