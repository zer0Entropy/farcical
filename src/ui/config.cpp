//
// Created by dgmuller on 7/6/25.
//
#include "../../include/ui/config.hpp"

#include "../../include/resource/parser.hpp"

std::expected<farcical::ui::DecorationConfig, farcical::engine::Error> farcical::ui::LoadDecorationConfig(
    const nlohmann::json& json) {
    DecorationConfig config;
    const auto& findID{json.find("id")};
    const auto& findPosition{json.find("relativePosition")};
    const auto& findTexture{json.find("texture")};

    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: WidgetID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if widgetID not found
    config.id = findID.value().get<std::string>();

    if(findPosition == json.end()) {
        const std::string failMsg{"Invalid configuration: Position could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if relativePosition not found
    const auto& loadPosition{jsonParser::ParsePosition(findPosition.value())};
    if(loadPosition.has_value()) {
        config.relativePosition = loadPosition.value();
    } // if loadPosition == success

    if(findTexture == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if textureID not found
    config.textureProperties.id = findTexture.value().get<std::string>();
    return config;
}

std::expected<farcical::ui::ButtonConfig, farcical::engine::Error> farcical::ui::LoadButtonConfig(
    const nlohmann::json& json) {
    ButtonConfig config;
    const auto& findState{json.find("state")};
    const auto& findTexture{json.find("texture")};

    if(findState == json.end()) {
        const std::string failMsg{"Invalid configuration: Button state could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if state not found
    const std::string buttonStateString{findState.value().get<std::string>()};

    const Button::Status buttonState{Button::GetStatusByName(buttonStateString)};

    if(findTexture == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if texture not found
    TextureProperties textureProperties;
    textureProperties.id = findTexture.value().get<std::string>();

    config.textureProperties.emplace_back(buttonState, textureProperties);
    return config;
}

std::expected<farcical::ui::TextConfig, farcical::engine::Error> farcical::ui::LoadTextConfig(
    const nlohmann::json& json) {
    TextConfig config;
    const auto& findID{json.find("id")};
    const auto& findPosition{json.find("relativePosition")};
    const auto& findFont{json.find("font")};
    const auto& findContents{json.find("contents")};

    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: WidgetID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if widgetID not found
    config.id = findID.value().get<std::string>();

    if(findPosition == json.end()) {
        const std::string failMsg{"Invalid configuration: Position could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if relativePosition not found
    const auto& loadPosition{jsonParser::ParsePosition(findPosition.value())};
    if(loadPosition.has_value()) {
        config.relativePosition = loadPosition.value();
    } // if loadPosition == success

    if(findFont == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if font not found
    config.fontProperties.id = findFont.value().get<std::string>();

    if(findContents == json.end()) {
        const std::string failMsg{"Invalid configuration: Text contents could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if contents not found
    config.contents = findContents.value().get<std::string>();

    return config;
}

std::expected<farcical::ui::MenuItemConfig, farcical::engine::Error> farcical::ui::LoadMenuItemConfig(
    const nlohmann::json& json) {
    MenuItemConfig config;
    const auto& findID{json.find("id")};
    const auto& findLabel{json.find("label")};
    const auto& findEventType{json.find("eventType")};
    const auto& findEventArgs{json.find("eventArgs")};
    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: WidgetID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if widgetID not found
    config.id = findID.value().get<std::string>();

    if(findLabel == json.end()) {
        const std::string failMsg{"Invalid configuration: MenuItem label could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if label not found
    config.labelConfig.contents = findLabel.value().get<std::string>();

    if(findEventType == json.end()) {
        const std::string failMsg{"Invalid configuration: MenuItem EventType could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if eventType not found
    if(findEventArgs == json.end()) {
        const std::string failMsg{"Invalid configuration: MenuItem EventType could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if eventArgs not found

    const std::string eventTypeName{findEventType.value().get<std::string>()};
    config.activationEventType = engine::Event::GetTypeByName(eventTypeName);

    const auto& eventArgsJSON{findEventArgs.value()};
    for(const auto& arg: eventArgsJSON) {
        if(arg.type() == nlohmann::json::value_t::string) {
            config.activationEventArgs.emplace_back(arg.get<std::string>());
        } // if string
        else if(arg.type() == nlohmann::json::value_t::number_integer) {
            config.activationEventArgs.emplace_back(arg.get<int>());
        } // else if integer
        else if(arg.type() == nlohmann::json::value_t::number_float) {
            config.activationEventArgs.emplace_back(arg.get<float>());
        } // else if float
    } // for each arg in eventArgsJSON

    return config;
}

std::expected<farcical::ui::MenuConfig, farcical::engine::Error> farcical::ui::LoadMenuConfig(
    const nlohmann::json& json) {
    MenuConfig config;
    const auto& findID{json.find("id")};
    const auto& findPosition{json.find("relativePosition")};
    const auto& findOrientation{json.find("orientation")};
    const auto& findRelativeSpacing{json.find("relativeSpacing")};
    const auto& findFont{json.find("font")};
    const auto& findButtons{json.find("buttons")};
    const auto& findItems{json.find("items")};
    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: WidgetID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if widgetID not found
    config.id = findID.value().get<std::string>();

    if(findPosition == json.end()) {
        const std::string failMsg{"Invalid configuration: Position could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if relativePosition not found
    const auto& loadPosition{jsonParser::ParsePosition(findPosition.value())};
    if(loadPosition.has_value()) {
        config.relativePosition = loadPosition.value();
    } // if loadPosition == success

    if(findOrientation == json.end()) {
        const std::string failMsg{"Invalid configuration: Orientation could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if orientation not found
    config.layout.orientation = MenuLayout::GetOrientationByName(findOrientation.value().get<std::string>());

    if(findRelativeSpacing == json.end()) {
        const std::string failMsg{"Invalid configuration: Spacing could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if relativeSpacing not found
    config.layout.relativeSpacing = findRelativeSpacing.value().get<int>();

    if(findFont == json.end()) {
        const std::string failMsg{"Invalid configuration: ResourceID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if font not found
    config.fontProperties.id = findFont.value().get<std::string>();

    if(findButtons == json.end()) {
        const std::string failMsg{"Invalid configuration: Menu Buttons could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    }
    const auto& buttonsJSON{findButtons.value()};
    for(const auto& buttonJSON: buttonsJSON) {
        const auto& loadButtonResult{LoadButtonConfig(buttonJSON)};
        if(loadButtonResult.has_value()) {
            const ButtonConfig buttonConfig{loadButtonResult.value()};
            for(const auto& stateTexturePair: buttonConfig.textureProperties) {
                config.buttonConfig.textureProperties.push_back(stateTexturePair);
            } // for each state-texture pair in buttonConfig
        } // if loadButtonResult == success
    } // for each buttonState-textureID pair in buttonsJSON

    if(findItems == json.end()) {
        const std::string failMsg{"Invalid configuration: MenuItems could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if menuItems not found
    const auto& itemsJSON{findItems.value()};
    for(const auto& itemJSON: itemsJSON) {
        const auto& loadItemResult{LoadMenuItemConfig(itemJSON)};
        if(loadItemResult.has_value()) {
            MenuItemConfig itemConfig{loadItemResult.value()};
            itemConfig.buttonConfig = config.buttonConfig;
            itemConfig.buttonConfig.id = itemConfig.id + "Button";
            itemConfig.labelConfig.id = itemConfig.id + "Label";
            itemConfig.labelConfig.fontProperties = config.fontProperties;
            config.menuItemConfigs.push_back(itemConfig);
        } // if loadItemResult == success
    } // for each item in itemsJSON

    return config;
}

std::expected<farcical::ui::LayoutLayerConfig, farcical::engine::Error> farcical::ui::LoadLayoutLayerConfig(
    const nlohmann::json& json) {
    LayoutLayerConfig config;
    const auto& findID{json.find("id")};
    const auto& findDecorations{json.find("decorations")};
    const auto& findTitle{json.find("title")};
    const auto& findHeadings{json.find("headings")};
    const auto& findMenu{json.find("menu")};
    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: LayerID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if LayerID not found
    const std::string layerName{findID.value().get<std::string>()};
    config.id = Layout::Layer::GetLayerIDByName(layerName);

    if(findDecorations != json.end()) {
        const auto& decorationsJSON{findDecorations.value()};
        for(const auto& decorationJSON: decorationsJSON) {
            const auto& loadDecorationResult{LoadDecorationConfig(decorationJSON)};
            if(loadDecorationResult.has_value()) {
                config.decorations.push_back(loadDecorationResult.value());
            } // if loadDecorationResult == success
        } // for each decoration in decorationsJSON
    } // if decorations found

    if(findTitle != json.end()) {
        const auto& titleJSON{findTitle.value()};
        const auto& loadTitleResult{LoadTextConfig(titleJSON)};
        if(loadTitleResult.has_value()) {
            config.title = loadTitleResult.value();
        } // if loadTitleResult == success
    } // if title found

    if(findHeadings != json.end()) {
        const auto& headingsJSON{findHeadings.value()};
        for(const auto& headingJSON: headingsJSON) {
            const auto& loadHeadingResult{LoadTextConfig(headingJSON)};
            if(loadHeadingResult.has_value()) {
                config.headings.push_back(loadHeadingResult.value());
            } // if loadHeadingResult == success
        } // for each heading in headingsJSON
    } // if headings found

    if(findMenu != json.end()) {
        const auto& menuJSON{findMenu.value()};
        const auto& loadMenuResult{LoadMenuConfig(menuJSON)};
        if(loadMenuResult.has_value()) {
            config.menu = loadMenuResult.value();
        } // if loadMenuResult == success
    } // if menu found

    return config;
}

std::expected<farcical::ui::LayoutConfig, farcical::engine::Error> farcical::ui::LoadLayoutConfig(
    const nlohmann::json& json) {
    LayoutConfig config;
    for(const auto& layerJSON: json.items()) {
        const auto& loadLayerResult{LoadLayoutLayerConfig(layerJSON.value())};
        if(loadLayerResult.has_value()) {
            const LayoutLayerConfig& layerConfig{loadLayerResult.value()};
            config.layers[static_cast<int>(layerConfig.id)] = layerConfig;
        } // if loadLayerResult == success
    } // for each layer in layoutJSON
    return config;
}

std::expected<farcical::ui::SceneConfig, farcical::engine::Error> farcical::ui::LoadSceneConfig(
    const nlohmann::json& json) {
    SceneConfig config;
    const auto& findID{json.find("id")};
    const auto& findFonts{json.find("fonts")};
    const auto& findTextures{json.find("textures")};
    const auto& findRepeating{json.find("repeatingTextures")};
    const auto& findSegmented(json.find("segmentedTextures"));
    const auto& findBorder(json.find("borderTexture"));
    const auto& findLayout{json.find("layout")};
    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: SceneID could not be found."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if SceneID not found
    config.id = findID.value().get<std::string>();

    // FONTS
    if(findFonts != json.end()) {
        const auto& fontsJSON{findFonts.value()};
        for(const auto& fontJSON: fontsJSON.items()) {
            const auto& loadFontResult{LoadFontProperties(fontJSON.value())};
            if(loadFontResult.has_value()) {
                config.fonts.push_back(loadFontResult.value());
            } // if loadFontResult == success
        } // for each font in fontsJSON
    } // if fonts found

    // TEXTURES
    if(findTextures != json.end()) {
        const auto& texturesJSON{findTextures.value()};
        for(const auto& textureJSON: texturesJSON.items()) {
            const auto& loadTextureResult{LoadTextureProperties(textureJSON.value())};
            if(loadTextureResult.has_value()) {
                config.textures.push_back(loadTextureResult.value());
            } // if loadTextureResult == success
        } // for each texture in texturesJSON
    } // if textures found

    // REPEATING TEXTURES
    if(findRepeating != json.end()) {
        const auto& texturesJSON{findRepeating.value()};
        for(const auto& textureJSON: texturesJSON.items()) {
            const auto& loadTextureResult{LoadRepeatingTextureProperties(textureJSON.value())};
            if(loadTextureResult.has_value()) {
                config.repeatingTextures.push_back(loadTextureResult.value());
            } // if loadTextureResult == success
        } // for each texture in texturesJSON
    } // if repeatingTextures found

    // SEGMENTED TEXTURES
    if(findSegmented != json.end()) {
        const auto& segmentedJSON{findSegmented.value()};
        for(const auto& segmentedTextureJSON: segmentedJSON) {
            const auto& loadPropertiesResult{LoadSegmentedTextureProperties(segmentedTextureJSON)};
            if(loadPropertiesResult.has_value()) {
                SegmentedTextureProperties segmentedTextureProperties{loadPropertiesResult.value()};
                const auto& findSegments{segmentedTextureJSON.find("segments")};
                if(findSegments != segmentedTextureJSON.end()) {
                    const auto& segmentListJSON{findSegments.value()};
                    std::string segmentListString{segmentListJSON.dump()};
                    for(auto& segmentProperties: segmentedTextureProperties.segments) {
                        const char firstLetter{segmentProperties.id.at(0)};
                        segmentProperties.id = static_cast<char>(std::toupper(firstLetter)) + segmentProperties.id.
                                               substr(1, segmentProperties.id.size() - 1);
                        segmentProperties.id = segmentedTextureProperties.id + segmentProperties.id;
                        segmentProperties.id.append("Texture");
                    } // for each segmentTexture in segmentedTexture
                } // if findSegments
                segmentedTextureProperties.id.append("Texture");
                config.segmentedTextures.push_back(segmentedTextureProperties);
            } // if loadPropertiesResult == success
        } // for each segmentedTexture in segmentedJSON
    } // if segmentedTextures found

    // BORDER TEXTURE
    if(findBorder != json.end()) {
        const auto& borderJSON{findBorder.value()};
        const auto& loadBorderResult{LoadBorderTextureProperties(borderJSON)};
        if(loadBorderResult.has_value()) {
            config.borderTexture = loadBorderResult.value();
        } // if loadBorderResult == success
    } // if borderTexture found

    // LAYOUT
    if(findLayout != json.end()) {
        const auto& layoutJSON{findLayout.value()};
        const auto& loadLayoutResult{LoadLayoutConfig(layoutJSON)};
        if(loadLayoutResult.has_value()) {
            config.layout = loadLayoutResult.value();

            for(auto& layer: config.layout.layers) {
                // Locate all Decorations for this Layer, if any.
                // If found, apply the correct textureProperties that were previously loaded:
                if(!layer.decorations.empty()) {
                    for(auto& decoration: layer.decorations) {
                        ResourceID textureID{decoration.textureProperties.id};
                        if(textureID == config.borderTexture.id) {
                            decoration.textureProperties = TextureProperties{
                                config.borderTexture.id,
                                config.borderTexture.path,
                                config.borderTexture.scale,
                                sf::IntRect{{0, 0}, {0, 0}}
                            };
                        } // if textureID == borderTextureID
                        else {
                            const auto& findTextureProperties{config.FindTextureProperties(textureID)};
                            const auto& findRepeatingProperties{config.FindRepeatingTextureProperties(textureID)};
                            const auto& findSegmentedProperties{config.FindSegmentedTextureProperties(textureID)};
                            if(findTextureProperties) {
                                decoration.textureProperties = findTextureProperties.value();
                            } // if findTextureProperties
                            else if(findRepeatingProperties) {
                                const auto& repeatingProperties{findRepeatingProperties.value()};
                                decoration.textureProperties = TextureProperties{
                                    textureID,
                                    repeatingProperties.path,
                                    repeatingProperties.scale,
                                    repeatingProperties.inputRect
                                };
                            } // else if findRepeatingProperties
                            else if(findSegmentedProperties) {
                                const auto& segmentedProperties{findSegmentedProperties.value()};
                                decoration.textureProperties = TextureProperties{
                                    textureID,
                                    segmentedProperties.path,
                                    segmentedProperties.scale,
                                    sf::IntRect{{0, 0}, {0, 0}}
                                };
                            } // else if findSegmentedProperties
                        } // else textureID != borderTexture
                    } // for each decoration
                } // if this layer has decorations

                // Locate the Title for this Layer, if any.
                // If found, apply the correct fontProperties that were previously loaded:
                if(!layer.title.id.empty()) {
                    const auto& findFontProperties{config.FindFontProperties(layer.title.fontProperties.id)};
                    if(findFontProperties.has_value()) {
                        layer.title.fontProperties = findFontProperties.value();
                    } // if fontProperties found
                } // if this layer has a title

                // Locate the Menu for this Layer, if any.
                // If found, go through each MenuItem and set the correct fontProperties for its Label:
                if(!layer.menu.id.empty()) {
                    for(auto& menuItemConfig: layer.menu.menuItemConfigs) {
                        const auto& findFontProperties{
                            config.FindFontProperties(menuItemConfig.labelConfig.fontProperties.id)
                        };
                        if(findFontProperties.has_value()) {
                            menuItemConfig.labelConfig.fontProperties = findFontProperties.value();
                        }
                    } // for each menuItemConfig in layer.menu
                } // if this layer has a menu
            } // for each layer in config.layout.layers
        } // if loadLayoutResult == success
    } // if layout found

    return config;
}

