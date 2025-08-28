//
// Created by dgmuller on 7/6/25.
//
#include "../../include/ui/config.hpp"

#include "../../include/resource/parser.hpp"

std::expected<farcical::ui::WidgetProperties, farcical::engine::Error> farcical::ui::LoadTextProperties(
    const nlohmann::json& json, engine::EntityID parentID) {
    WidgetProperties properties;
    properties.type = Widget::Type::Text;
    properties.parentID = parentID;
    /**********************     ID      **********************/
    const auto& extractIDResult{ExtractWidgetID(json, parentID)};
    if(!extractIDResult.has_value()) {
        return std::unexpected(extractIDResult.error());
    } // if extractIDResult == failure
    properties.id = extractIDResult.value();
    /**********************     ID      **********************/

    /**********************     fontID      **********************/
    const auto& extractFontIDResult{ExtractFontID(json, parentID)};
    if(!extractFontIDResult.has_value()) {
        return std::unexpected(extractFontIDResult.error());
    } // if extractFontIDResult == failure
    properties.labelProperties.second.id = extractFontIDResult.value();
    /**********************     fontID      **********************/

    /**********************     contents      **********************/
    const auto& extractContentsResult{ExtractTextContents(json, parentID)};
    if(!extractContentsResult.has_value()) {
        return std::unexpected(extractContentsResult.error());
    } // if extractContentsResult == failure
    properties.labelProperties.first = extractContentsResult.value();
    /**********************     contents      **********************/

    /**********************     relativePosition      **********************/
    const auto& extractPositionResult{ExtractRelativePosition(json, parentID)};
    if(!extractPositionResult.has_value()) {
        return std::unexpected(extractPositionResult.error());
    } // if extractPositionResult == failure
    properties.relativePosition = extractPositionResult.value();
    /**********************     relativePosition      **********************/
    return properties;
}

std::expected<farcical::ui::DecorationProperties, farcical::engine::Error> farcical::ui::LoadDecoration(
    const nlohmann::json& json, engine::EntityID parentID) {
    DecorationProperties properties;
    properties.type = Widget::Type::Decoration;
    properties.parentID = parentID;
    /**********************     ID      **********************/
    const auto& extractIDResult{ExtractWidgetID(json, Widget::GetTypeName(properties.type))};
    if(!extractIDResult.has_value()) {
        return std::unexpected(extractIDResult.error());
    } // if extractIDResult == failure
    properties.id = extractIDResult.value();
    /**********************     ID      **********************/

    /**********************     relativePosition      **********************/
    const auto& extractPositionResult{ExtractRelativePosition(json, properties.id)};
    if(!extractPositionResult.has_value()) {
        return std::unexpected(extractPositionResult.error());
    } // if extractPositionResult == failure
    properties.relativePosition = extractPositionResult.value();
    /**********************     relativePosition      **********************/

    /**********************     textureID      **********************/
    const auto& extractTextureResult{ExtractTextureID(json, properties.id)};
    if(!extractTextureResult.has_value()) {
        return std::unexpected(extractTextureResult.error());
    } // if extractTextureResult == failure
    properties.textureProperties.id = extractTextureResult.value();
    /**********************     textureID      **********************/
    return properties;
}

std::expected<farcical::ui::ButtonProperties, farcical::engine::Error> farcical::ui::LoadButton(
    const nlohmann::json& json, engine::EntityID parentID) {
    ButtonProperties properties;
    properties.type = Widget::Type::Button;
    properties.parentID = parentID;
    /**********************     ID      **********************/
    const auto& extractIDResult{ExtractWidgetID(json, Widget::GetTypeName(properties.type))};
    if(!extractIDResult.has_value()) {
        return std::unexpected(extractIDResult.error());
    } // if extractIDResult == failure
    properties.id = extractIDResult.value();
    /**********************     ID      **********************/

    /**********************     labelContents      **********************/
    const auto& extractContentsResult{ExtractTextContents(json, properties.id)};
    if(!extractContentsResult.has_value()) {
        return std::unexpected(extractContentsResult.error());
    } // if extractContentsResult == failure
    properties.labelProperties.first = extractContentsResult.value();
    /**********************     labelContents      **********************/

    /**********************     onPressEvent      **********************/
    const auto& extractEventResult{ExtractButtonEvent(json, properties.id)};
    if(!extractEventResult.has_value()) {
        return std::unexpected(extractEventResult.error());
    } // if extractEventResult == failure
    properties.onPressEvent = extractEventResult.value();
    /**********************     onPressEvent      **********************/
    return properties;
}

std::expected<farcical::ui::WidgetProperties, farcical::engine::Error> farcical::ui::LoadRadioButton(
    const nlohmann::json& json, engine::EntityID parentID) {
    WidgetProperties properties;
    properties.type = Widget::Type::RadioButton;
    properties.parentID = parentID;
    /**********************     ID      **********************/
    const auto& extractIDResult{ExtractWidgetID(json, Widget::GetTypeName(properties.type))};
    if(!extractIDResult.has_value()) {
        return std::unexpected(extractIDResult.error());
    } // if extractIDResult == failure
    properties.id = extractIDResult.value();
    /**********************     ID      **********************/

    /**********************     labelContents      **********************/
    const auto& extractContentsResult{ExtractTextContents(json, properties.id)};
    if(!extractContentsResult.has_value()) {
        return std::unexpected(extractContentsResult.error());
    } // if extractContentsResult == failure
    properties.labelProperties.first = extractContentsResult.value();
    /**********************     labelContents      **********************/
    return properties;
}

std::expected<farcical::ui::MenuProperties, farcical::engine::Error> farcical::ui::LoadMenu(const nlohmann::json& json,
    engine::EntityID parentID) {
    MenuProperties properties;
    properties.type = Widget::Type::Menu;
    properties.parentID = parentID;
    /**********************     ID      **********************/
    const auto& extractIDResult{ExtractWidgetID(json, Widget::GetTypeName(properties.type))};
    if(!extractIDResult.has_value()) {
        return std::unexpected(extractIDResult.error());
    } // if extractIDResult == failure
    properties.id = extractIDResult.value();
    /**********************     ID      **********************/

    /**********************     MenuType      **********************/
    const auto& extractTypeResult{ExtractMenuType(json, properties.id)};
    if(!extractTypeResult.has_value()) {
        return std::unexpected(extractTypeResult.error());
    } // if extractTypeResult == failure
    properties.menuType = extractTypeResult.value();
    /**********************     MenuType      **********************/

    /**********************     FontID      **********************/
    const auto& extractFontResult{ExtractFontID(json, properties.id)};
    if(!extractFontResult.has_value()) {
        return std::unexpected(extractFontResult.error());
    } // if extractFontResult == failure
    properties.labelProperties.second.id = extractFontResult.value();
    /**********************     FontID      **********************/

    /**********************     Orientation      **********************/
    const auto& extractOrientationResult{ExtractOrientation(json, properties.id)};
    if(!extractOrientationResult.has_value()) {
        return std::unexpected(extractOrientationResult.error());
    } // if extractOrientationResult == failure
    properties.layout.orientation = extractOrientationResult.value();
    /**********************     Orientation      **********************/

    /**********************     relativeSpacing      **********************/
    const auto& extractSpacingResult{ExtractRelativeSpacing(json, properties.id)};
    if(!extractSpacingResult.has_value()) {
        return std::unexpected(extractSpacingResult.error());
    } // if extractSpacingResult == failure
    properties.layout.relativeSpacing = extractSpacingResult.value();
    /**********************     relativeSpacing      **********************/

    /**********************     relativePosition      **********************/
    const auto& extractPositionResult{ExtractRelativePosition(json, properties.id)};
    if(!extractPositionResult.has_value()) {
        return std::unexpected(extractPositionResult.error());
    } // if extractPositionResult == failure
    properties.relativePosition = extractPositionResult.value();
    /**********************     relativePosition      **********************/

    if(properties.menuType == Menu::Type::Button) {
        /**********************     buttonTextures      **********************/
        const auto& extractTexturesResult{ExtractButtonTextures(json, properties.id)};
        if(!extractTexturesResult.has_value()) {
            return std::unexpected(extractTexturesResult.error());
        } // if extractTexturesResult == failure
        properties.buttonTextures = extractTexturesResult.value();
        /**********************     buttonTextures      **********************/

        /**********************     buttonProperties      **********************/
        const auto& extractPropertiesResult{ExtractButtonProperties(json, properties)};
        if(!extractPositionResult.has_value()) {
            return std::unexpected(extractPropertiesResult.error());
        } // if extractPropertiesResult == failure
        properties.buttonProperties = extractPropertiesResult.value();
        /**********************     buttonProperties      **********************/
    } // if menuType == Button
    else if(properties.menuType == Menu::Type::RadioButton) {
        /**********************     radioButtonTextures      **********************/
        const auto& extractTexturesResult{ExtractRadioButtonTextures(json, properties.id)};
        if(!extractTexturesResult.has_value()) {
            return std::unexpected(extractTexturesResult.error());
        } // if extractTexturesResult == failure
        properties.radioButtonTextures = extractTexturesResult.value();
        /**********************     radioButtonTextures      **********************/

        /**********************     radioButtonProperties      **********************/
        const auto& extractPropertiesResult{ExtractRadioButtonProperties(json, properties)};
        if(!extractPropertiesResult.has_value()) {
            return std::unexpected(extractPropertiesResult.error());
        } // if extractPropertiesResult == failure
        properties.radioButtonProperties = extractPropertiesResult.value();
        /**********************     radioButtonProperties      **********************/
    } // else if menuType == RadioButton
    else if(properties.menuType == Menu::Type::SubMenu) {
        /**********************     menuProperties      **********************/
        const auto& findMenus{json.find("menus")};
        if(findMenus != json.end()) {
            const auto& menusJSON{findMenus.value()};
            for(const auto& menuJSON: menusJSON) {
                const auto& loadSubMenuResult{LoadMenu(menuJSON, properties.id)};
                if(!loadSubMenuResult.has_value()) {
                    return std::unexpected(loadSubMenuResult.error());
                } // if loadSubMenuResult == failure
                properties.menuProperties.push_back(loadSubMenuResult.value());
            } // for each subMeu in menusJSON
        } // if menus found
        /**********************     menuProperties      **********************/
    } // else if menuType == SubMenu
    return properties;
}

std::expected<farcical::ui::LayoutLayerProperties, farcical::engine::Error> farcical::ui::LoadLayoutLayer(
    const nlohmann::json& json, engine::EntityID parentID) {
    LayoutLayerProperties properties;
    /**********************     layerID      **********************/
    const auto& extractLayerIDResult{ExtractLayerID(json, parentID)};
    if(!extractLayerIDResult.has_value()) {
        return std::unexpected(extractLayerIDResult.error());
    } // if extractLayerIDResult == failure
    properties.id = extractLayerIDResult.value();
    /**********************     layerID      **********************/

    /**********************     Decorations      **********************/
    const auto& findDecorations{json.find("decorations")};
    if(findDecorations != json.end()) {
        const auto& decorationsJSON{findDecorations.value()};
        for(const auto& decorationJSON: decorationsJSON) {
            const std::string jsonDump{decorationJSON.dump()};
            const auto& loadDecorationResult{LoadDecoration(decorationJSON, parentID)};
            if(!loadDecorationResult.has_value()) {
                return std::unexpected(loadDecorationResult.error());
            } // if loadDecorationResult == failure
            DecorationProperties decorationProperties{loadDecorationResult.value()};
            decorationProperties.layerID = properties.id;
            properties.decorationProperties.push_back(decorationProperties);
        } // for each Decoration
    } // if Decorations found
    /**********************     Decorations      **********************/

    /**********************     Title      **********************/
    const auto& findTitle{json.find("title")};
    if(findTitle != json.end()) {
        const auto& titleJSON{findTitle.value()};
        const auto& loadTitleResult{LoadTextProperties(titleJSON, parentID)};
        if(!loadTitleResult.has_value()) {
            return std::unexpected(loadTitleResult.error());
        } // if loadTitleResult == failure
        WidgetProperties titleProperties{loadTitleResult.value()};
        titleProperties.layerID = properties.id;
        properties.titleProperties = titleProperties;
    } // if Title found
    /**********************     Title      **********************/

    /**********************     Headings      **********************/
    const auto& findHeadings{json.find("headings")};
    if(findHeadings != json.end()) {
        const auto& headingsJSON{findHeadings.value()};
        for(const auto& headingJSON: headingsJSON) {
            const auto& loadHeadingResult{LoadTextProperties(headingJSON, parentID)};
            if(!loadHeadingResult.has_value()) {
                return std::unexpected(loadHeadingResult.error());
            } // if loadHeadingResult == failure
            WidgetProperties headingProperties{loadHeadingResult.value()};
            headingProperties.layerID = properties.id;
            properties.headingProperties.push_back(headingProperties);
        } // for each Heading
    } // if Headings found
    /**********************     Headings      **********************/

    /**********************     Menu      **********************/
    const auto& findMenu(json.find("menu"));
    if(findMenu != json.end()) {
        const auto& menuJSON{findMenu.value()};
        const auto& loadMenuResult{LoadMenu(menuJSON, parentID)};
        if(!loadMenuResult.has_value()) {
            return std::unexpected(loadMenuResult.error());
        } // if loadMenuResult == failure
        MenuProperties menuProperties{loadMenuResult.value()};
        menuProperties.layerID = properties.id;
        properties.menuProperties = menuProperties;
    } // if Menu found
    /**********************     Menu      **********************/

    return properties;
}

std::expected<farcical::ui::LayoutProperties, farcical::engine::Error> farcical::ui::LoadLayout(
    const nlohmann::json& json, engine::EntityID parentID) {
    LayoutProperties properties;
    for(const auto& layerJSON: json.items()) {
        const auto& loadLayerResult{LoadLayoutLayer(layerJSON.value(), parentID)};
        if(!loadLayerResult.has_value()) {
            return std::unexpected(loadLayerResult.error());
        } // if loadLayerResult == failure
        const LayoutLayerProperties& layerProperties{loadLayerResult.value()};
        properties.layers[static_cast<int>(layerProperties.id)] = layerProperties;
    }
    return properties;
}

std::expected<farcical::ui::SceneProperties, farcical::engine::Error> farcical::ui::LoadScene(
    const nlohmann::json& json) {
    SceneProperties properties;
    /**********************     ID      **********************/
    const auto& findID{json.find("id")};
    if(findID == json.end()) {
        const std::string failMsg{"Invalid configuration: No ID found for Scene."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if ID not found
    properties.id = findID.value().get<std::string>();
    /**********************     ID      **********************/

    /**********************     Music      **********************/
    const auto& findMusic{json.find("music")};
    if(findMusic != json.end()) {
        const auto& musicJSON{findMusic.value()};
        const auto& loadMusicResult{LoadMusicProperties(musicJSON)};
        if(!loadMusicResult.has_value()) {
            return std::unexpected(loadMusicResult.error());
        } // if loadMusicResult == failure
        properties.music = loadMusicResult.value();
    } // if Music found
    /**********************     Music      **********************/

    /**********************     Fonts      **********************/
    const auto& findFonts{json.find("fonts")};
    if(findFonts != json.end()) {
        const auto& fontsJSON{findFonts.value()};
        for(const auto& fontJSON: fontsJSON) {
            const auto& loadFontResult{LoadFontProperties(fontJSON)};
            if(!loadFontResult.has_value()) {
                return std::unexpected(loadFontResult.error());
            } // if loadFontResult == failure
            properties.fonts.push_back(loadFontResult.value());
        } // for each Font
    } // if Fonts found
    /**********************     Fonts      **********************/

    /**********************     Textures      **********************/
    const auto& findTextures{json.find("textures")};
    if(findTextures != json.end()) {
        const auto& texturesJSON{findTextures.value()};
        for(const auto& textureJSON: texturesJSON) {
            const auto& loadTextureResult{LoadTextureProperties(textureJSON)};
            if(!loadTextureResult.has_value()) {
                return std::unexpected(loadTextureResult.error());
            } // if loadTextureResult == failure
            properties.textures.push_back(loadTextureResult.value());
        } // for each Texture
    } // if Textures found
    /**********************     Textures      **********************/

    /**********************     repeatingTextures      **********************/
    const auto& findRepeatingTextures{json.find("repeatingTextures")};
    if(findRepeatingTextures != json.end()) {
        const auto& repeatingTexturesJSON{findRepeatingTextures.value()};
        for(const auto& textureJSON: repeatingTexturesJSON) {
            const auto& loadTextureResult{LoadRepeatingTextureProperties(textureJSON)};
            if(!loadTextureResult.has_value()) {
                return std::unexpected(loadTextureResult.error());
            } // if loadTextureResult == failure
            properties.repeatingTextures.push_back(loadTextureResult.value());
        } // for each repeatingTexture
    } // if repeatingTextures found
    /**********************     repeatingTextures      **********************/

    /**********************     segmentedTextures      **********************/
    const auto& findSegmentedTextures{json.find("segmentedTextures")};
    if(findSegmentedTextures != json.end()) {
        const auto& segmentedTexturesJSON{findSegmentedTextures.value()};
        for(const auto& textureJSON: segmentedTexturesJSON) {
            const auto& loadTextureResult{LoadSegmentedTextureProperties(textureJSON)};
            if(!loadTextureResult.has_value()) {
                return std::unexpected(loadTextureResult.error());
            } // if loadTextureResult == failure
            properties.segmentedTextures.push_back(loadTextureResult.value());
        } // for each segmentedTexture
    } // if segmentedTextures found
    /**********************     segmentedTextures      **********************/

    /**********************     borderTexture      **********************/
    const auto& findBorderTexture{json.find("borderTexture")};
    if(findBorderTexture != json.end()) {
        const auto& borderTextureJSON{findBorderTexture.value()};
        const auto& loadTextureResult{LoadBorderTextureProperties(borderTextureJSON)};
        if(!loadTextureResult.has_value()) {
            return std::unexpected(loadTextureResult.error());
        } // if loadTextureResult == failure
        properties.borderTexture = loadTextureResult.value();
    } // if borderTexture found
    /**********************     borderTexture      **********************/

    /**********************     Layout      **********************/
    const auto& findLayout{json.find("layout")};
    if(findLayout == json.end()) {
        const std::string failMsg{"Invalid configuration: Layout not found for Scene with id=" + properties.id + "."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if Layout not found
    const auto& layoutJSON{findLayout.value()};
    const auto& loadLayoutResult{LoadLayout(layoutJSON, properties.id)};
    if(!loadLayoutResult.has_value()) {
        return std::unexpected(loadLayoutResult.error());
    } // if loadLayoutResult == failure
    properties.layout = loadLayoutResult.value();
    /**********************     Layout      **********************/

    return properties;
}

std::expected<farcical::engine::EntityID, farcical::engine::Error> farcical::ui::ExtractWidgetID(
    const nlohmann::json& json, std::string_view typeName) {
    const auto& findID{json.find("id")};
    if(findID == json.end()) {
        const std::string failMsg{
            "Invalid configuration: ID could not be found for Widget with type=\"" + std::string{typeName} + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if WidgetID not found
    return findID.value().get<engine::EntityID>();
}

std::expected<farcical::ui::Widget::Type, farcical::engine::Error> farcical::ui::ExtractWidgetType(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findType{json.find("type")};
    if(findType == json.end()) {
        const std::string failMsg{"Invalid configuration: Type could not be found for Widget with id=\"" + id + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if WidgetType could not be found
    return Widget::GetTypeByName(findType.value().get<std::string>());
}

std::expected<farcical::ui::Menu::Type, farcical::engine::Error> farcical::ui::ExtractMenuType(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findType{json.find("type")};
    if(findType == json.end()) {
        const std::string failMsg{"Invalid configuration: Type could not be found for Menu with id=\"" + id + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if MenuType could not be found
    return Menu::GetTypeByName(findType.value().get<std::string>());
}

std::expected<sf::Vector2i, farcical::engine::Error> farcical::ui::ExtractRelativePosition(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findPosition{json.find("relativePosition")};
    if(findPosition == json.end()) {
        const std::string failMsg{
            "Invalid configuration: Position could not be found for Widget with id=\"" + id + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if position not found
    const auto& positionJSON{findPosition.value()};
    const auto& findX{positionJSON.find("x")};
    const auto& findY{positionJSON.find("y")};
    if(findX == positionJSON.end() || findY == positionJSON.end()) {
        const std::string failMsg{
            "Invalid configuration: Invalid coordinates provided for Widget with id=\"" + id + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if either X or Y could not be found
    return sf::Vector2i{findX.value().get<int>(), findY.value().get<int>()};
}

std::expected<farcical::Orientation, farcical::engine::Error> farcical::ui::ExtractOrientation(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findOrientation{json.find("orientation")};
    if(findOrientation == json.end()) {
        const std::string failMsg{"Invalid configuration: Orientation could not be found for Widget with id=\"" + id + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if orientation not found
    return MenuLayout::GetOrientationByName(findOrientation.value().get<std::string>());
}

std::expected<int, farcical::engine::Error> farcical::ui::ExtractRelativeSpacing(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findSpacing{json.find("relativeSpacing")};
    if(findSpacing == json.end()) {
        const std::string failMsg{"Invalid configuration: Spacing could not be found for Widget with id=\"" + id + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if relativeSpacing not found
    return findSpacing.value().get<int>();
}

std::expected<farcical::ResourceID, farcical::engine::Error> farcical::ui::ExtractTextureID(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findTexture{json.find("texture")};
    if(findTexture == json.end()) {
        const std::string failMsg{
            "Invalid configuration: Texture could not be found for Widget with id=\"" + id + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if Texture not found
    return findTexture.value().get<ResourceID>();
}

std::expected<farcical::ResourceID, farcical::engine::Error> farcical::ui::ExtractFontID(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findFont{json.find("font")};
    if(findFont == json.end()) {
        const std::string failMsg{"Invalid configuration: Font could not be found for Widget with id=\"" + id + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if Font not found
    return findFont.value().get<ResourceID>();
}

std::expected<std::string, farcical::engine::Error> farcical::ui::ExtractTextContents(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findContents{json.find("contents")};
    if(findContents == json.end()) {
        const std::string failMsg{"Invalid configuration: Text contents could not be found for Widget with id=\"" + id + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if contents not found
    return findContents.value().get<std::string>();
}

std::expected<farcical::engine::Event::Parameters, farcical::engine::Error> farcical::ui::ExtractButtonEvent(
    const nlohmann::json& json, engine::EntityID id) {
    engine::Event::Parameters eventParameters;
    const auto& findOnPress{json.find("onPress")};
    if(findOnPress == json.end()) {
        const std::string failMsg{"Invalid configuration: onPressEvent could not be found for Button with id=\"" + id + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if onPressEvent not found
    const auto& onPressJSON{findOnPress.value()};
    const auto& findEventType{onPressJSON.find("type")};
    if(findEventType == onPressJSON.end()) {
        const std::string failMsg{
            "Invalid configuration: EventType could not be found in onPressEvent for Button with id=\"" + id + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if EventType not found
    eventParameters.type = engine::Event::GetTypeByName(findEventType.value().get<std::string>());
    const auto& findEventArgs{onPressJSON.find("args")};
    if(findEventType == onPressJSON.end()) {
        const std::string failMsg{
            "Invalid configuration: EventArgs could not be found in onPressEvent for Button with id=\"" + id + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if EventArgs not found
    const auto& argsJSON{findEventArgs.value()};
    for(const auto& arg: argsJSON) {
        if(arg.type() == nlohmann::json::value_t::string) {
            eventParameters.args.push_back(arg.get<std::string>());
        } // if arg.type == string
        else if(arg.type() == nlohmann::json::value_t::boolean) {
            eventParameters.args.push_back(arg.get<bool>());
        } // else if arg.type == bool
        else if(arg.type() == nlohmann::json::value_t::number_float) {
            eventParameters.args.push_back(arg.get<float>());
        } // else if arg.type == float
        else if(arg.type() == nlohmann::json::value_t::number_integer) {
            eventParameters.args.push_back(arg.get<int>());
        } // else if arg.type == integer
    } // for each EventArg
    return eventParameters;
}

std::expected<std::vector<farcical::ui::ButtonProperties>, farcical::engine::Error>
farcical::ui::ExtractButtonProperties(
    const nlohmann::json& json, const MenuProperties& menuProperties) {
    std::vector<ButtonProperties> propertiesList;
    const auto& findButtons{json.find("buttons")};
    if(findButtons == json.end()) {
        const std::string failMsg{"Invalid configuration: Buttons could not be found for Menu with id=\"" +
                                  menuProperties.id + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if buttons not found
    const auto& buttonsJSON{findButtons.value()};
    for(const auto& buttonJSON: buttonsJSON) {
        const auto& loadButtonResult{LoadButton(buttonJSON, menuProperties.id)};
        if(!loadButtonResult.has_value()) {
            return std::unexpected(loadButtonResult.error());
        } // if loadButtonResult == failure
        ButtonProperties buttonProperties{loadButtonResult.value()};
        buttonProperties.parentID = menuProperties.id;
        buttonProperties.relativePosition = menuProperties.relativePosition;
        propertiesList.push_back(buttonProperties);
    } // for each Button in ButtonsJSON
    return propertiesList;
}

std::expected<std::vector<farcical::ui::WidgetProperties>, farcical::engine::Error>
farcical::ui::ExtractRadioButtonProperties(
    const nlohmann::json& json, const MenuProperties& menuProperties) {
    std::vector<WidgetProperties> propertiesList;
    const auto& findRadioButtons{json.find("radioButtons")};
    if(findRadioButtons == json.end()) {
        const std::string failMsg{
            "Invalid configuration: RadioButtons could not be found for Menu with id=\"" +
            menuProperties.id + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    }
    const auto& radioButtonsJSON{findRadioButtons.value()};
    for(const auto& radioButtonJSON: radioButtonsJSON) {
        const auto& loadRadioButtonResult{LoadRadioButton(radioButtonJSON, menuProperties.id)};
        if(!loadRadioButtonResult.has_value()) {
            return std::unexpected(loadRadioButtonResult.error());
        } // if loadRadioButtonResult == failure
        WidgetProperties radioButtonProperties{loadRadioButtonResult.value()};
        radioButtonProperties.parentID = menuProperties.id;
        radioButtonProperties.relativePosition = menuProperties.relativePosition;
        propertiesList.push_back(radioButtonProperties);
    } // for each RadioButton in RadioButtonsJSON
    return propertiesList;
}

std::expected<std::vector<std::pair<farcical::ui::Button::Status, farcical::TextureProperties> >,
    farcical::engine::Error>
farcical::ui::ExtractButtonTextures(const nlohmann::json& json, engine::EntityID id) {
    std::vector<std::pair<Button::Status, TextureProperties> > buttonTextures;
    const auto& findTextures{json.find("buttonTextures")};
    if(findTextures == json.end()) {
        const std::string failMsg{"Invalid configuration: buttonTextures could not be found for Menu with id=\"" + id +
                                  "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if findTextures == failure
    const auto& texturesJSON{findTextures.value()};
    for(const auto& texture: texturesJSON) {
        const auto& findState{texture.find("state")};
        const auto& findTextureID{texture.find("texture")};
        if(findState == texture.end() || findTextureID == texture.end()) {
            const std::string failMsg{"Invalid configuration in buttonTextures for Menu with id=\"" + id + "\"."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if either "state" or "texture" is missing
        Button::Status status{Button::GetStatusByName(findState.value().get<std::string>())};
        const ResourceID textureID{findTextureID.value().get<std::string>()};
        TextureProperties textureProperties;
        textureProperties.id = textureID;
        buttonTextures.emplace_back(status, textureProperties);
    } // for each Texture
    return buttonTextures;
}

std::expected<std::vector<std::pair<farcical::ui::RadioButton::Status, farcical::TextureProperties> >,
    farcical::engine::Error>
farcical::ui::ExtractRadioButtonTextures(const nlohmann::json& json, engine::EntityID id) {
    std::vector<std::pair<RadioButton::Status, TextureProperties> > radioButtonTextures;
    const auto& findTextures{json.find("radioButtonTextures")};
    if(findTextures == json.end()) {
        const std::string failMsg{
            "Invalid configuration: radioButtonTextures could not be found for Menu with id=\"" + id + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if findTextures == failure
    const auto& texturesJSON{findTextures.value()};
    for(const auto& texture: texturesJSON) {
        const auto& findState{texture.find("state")};
        const auto& findTextureID{texture.find("texture")};
        if(findState == texture.end() || findTextureID == texture.end()) {
            const std::string failMsg{"Invalid configuration in radioButtonTextures for Menu with id=\"" + id + "\"."};
            return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
        } // if either "state" or "texture" is missing
        RadioButton::Status status{RadioButton::GetStatusByName(findState.value().get<std::string>())};
        const ResourceID textureID{findTextureID.value().get<std::string>()};
        TextureProperties textureProperties;
        textureProperties.id = textureID;
        radioButtonTextures.emplace_back(status, textureProperties);
    } // for each Texture
    return radioButtonTextures;
}

std::expected<farcical::ui::Layout::Layer::ID, farcical::engine::Error> farcical::ui::ExtractLayerID(
    const nlohmann::json& json, engine::EntityID id) {
    const auto& findID{json.find("id")};
    if(findID == json.end()) {
        const std::string failMsg{
            "Invalid configuration: layerID not found in Layout for Menu with id=\"" + id + "\"."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if ID not found
    const std::string layerName{findID.value().get<std::string>()};
    Layout::Layer::ID layerID{Layout::Layer::GetLayerIDByName(layerName)};
    return layerID;
}
