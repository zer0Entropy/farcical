//
// Created by dgmuller on 6/7/25.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include "../../include/game/game.hpp"
#include "../../include/ui/menu.hpp"
#include "../../include/ui/decoration.hpp"


farcical::game::Game::Game(engine::Engine& engine):
    engine{engine},
    currentState{State::ID::MainMenu} {
}

std::optional<farcical::engine::Error> farcical::game::Game::Init() {
    TransitionToState(State::ID::MainMenu);
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::Update() {
    return std::nullopt;
}

void farcical::game::Game::TransitionToState(State::ID stateID) {
    State::ID previousStateID{currentState.id};
    currentState.id = stateID;
    switch(stateID) {
        case State::ID::MainMenu: {
            const ResourceHandle* documentHandle{engine.GetResourceManager().GetResourceHandle(ResourceID{mainMenuSceneID})};
            if(!documentHandle) {
                const auto& createDocumentHandle{
                    engine.GetResourceManager().CreateResourceHandle(ResourceID{mainMenuSceneID}, ResourceHandle::Type::JSONDocument, mainMenuPath)
                };
                if(createDocumentHandle.has_value()) {
                    documentHandle = createDocumentHandle.value();
                } // if createDocumentHandle == success
            } // if !documentHandle
            const auto& requestJSONDoc{engine.GetResourceManager().GetJSONDoc(documentHandle->id)};
            if(requestJSONDoc.has_value()) {
                const auto& sceneJSON{requestJSONDoc.value()};
                const auto& loadSceneConfig{ui::LoadSceneConfig(*sceneJSON)};
                if(loadSceneConfig.has_value()) {
                    const auto& sceneConfig{loadSceneConfig.value()};
                    auto createScene{CreateScene(sceneConfig, nullptr)};
                    sceneHierarchy.root = std::move(createScene.value());
                    sceneHierarchy.currentScene = sceneHierarchy.root.get();
                } // if loadScene == success
            } // if requestJSONDoc == success
        }
        break;
        case State::ID::StartNewGame: {
            /*
            currentState.world = std::move(CreateWorld());
            currentState.player = std::move(CreatePlayer());
            TransitionToState(State::ID::Gameplay);
            */
        }
        break;
        case State::ID::LoadSavedGame: {
        } break;
        case State::ID::OptionsMenu: {
        }
        break;
        case State::ID::Gameplay: {
            //ui::Scene* scene{LoadScene(Game::gameplayPath)};
        } break;
    }
}

std::unique_ptr<farcical::game::World> farcical::game::Game::CreateWorld() {
    return std::make_unique<World>();
}

std::unique_ptr<farcical::game::Player> farcical::game::Game::CreatePlayer() {
    return std::make_unique<Player>();
}

std::expected<std::unique_ptr<farcical::ui::Scene>, farcical::engine::Error> farcical::game::Game::CreateScene(ui::SceneConfig config, ui::Scene* parent) {
    std::unique_ptr<ui::Scene> scene{std::make_unique<ui::Scene>(config.id, parent, sceneHierarchy)};
    std::unordered_map<ResourceID, sf::Texture*> textureCache;
    std::unordered_map<ResourceID, TextureProperties> texturePropertiesCache;
    const sf::Vector2u& windowSize{engine.GetConfig().windowProperties.sizeInPixels};

    /***************           CACHE RESOURCES         ***************/
    /*  FONTS  */
    CacheFonts(*scene, config.fonts);
    /*  TEXTURES  */
    CacheTextures(*scene, config.textures);
    /*  REPEATING TEXTURES  */
    CacheRepeatingTextures(*scene, config.repeatingTextures);
    /*  SEGMENTED TEXTURES  */
    CacheSegmentedTextures(*scene, config.segmentedTextures);
    /*****************************************************************/

    /*  CREATE THE LAYOUT   */
    CreateSceneLayout(*scene, config.layout);

    return std::move(scene);
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateSceneLayout(ui::Scene& scene, const ui::LayoutConfig& layoutConfig) {
    for(const auto& layerConfig: layoutConfig.layers) {
        std::string layerName{layerConfig.layerID};
        ui::Layout::Layer::ID layerID{ui::Layout::Layer::GetLayerIDByName(layerName)};
        CreateDecorations(scene, layerConfig.decorations);
        CreateTitle(scene, layerConfig.title);
        CreateMenu(scene, layerConfig.menu);
        for(const auto& child: scene.GetChildren()) {
            ui::Widget::Type widgetType{child->GetType()};
            if(widgetType == ui::Widget::Type::Decoration) {
                ui::Decoration* decoration{dynamic_cast<ui::Decoration*>(child)};
                const auto& createRenderCmp{
                    engine.GetRenderSystem().CreateRenderComponent(layerID, decoration->GetID(), decoration->GetTexture())
                };
                if(createRenderCmp.has_value()) {
                    engine::RenderComponent* renderCmp{createRenderCmp.value()};
                    renderCmp->scale = decoration->GetScale();
                    renderCmp->position = decoration->GetPosition();
                    decoration->AddComponent(renderCmp);
                } // if createRenderCmp == success
            } // if Decoration
            else if(widgetType == ui::Widget::Type::Label) {
                ui::Label* label{dynamic_cast<ui::Label*>(child)};
                const auto& createRenderCmp{
                    engine.GetRenderSystem().CreateRenderComponent(layerID, label->GetID(), label->GetFont(), label->GetFontProperties(), label->GetContents())
                };
                if(createRenderCmp.has_value()) {
                    engine::RenderComponent* renderCmp{createRenderCmp.value()};
                    renderCmp->scale = label->GetScale();
                    renderCmp->position = label->GetPosition();
                    label->AddComponent(renderCmp);
                } // if createRenderCmp == success
            } // else if Label
            else if(widgetType == ui::Widget::Type::Menu) {
                ui::Menu* menu{dynamic_cast<ui::Menu*>(child)};
                for(int index = 0; index < menu->GetNumMenuItems(); ++index) {
                    ui::MenuItem* item{menu->GetMenuItemByIndex(index)};
                    ui::Button* button{item->GetButton()};
                    ui::Label* label{item->GetLabel()};
                    const auto& createButtonRenderCmp{
                        engine.GetRenderSystem().CreateRenderComponent(layerID, button->GetID(), button->GetTexture())
                    };
                    if(createButtonRenderCmp.has_value()) {
                        engine::RenderComponent* buttonRenderCmp{createButtonRenderCmp.value()};
                        buttonRenderCmp->scale = button->GetScale();
                        buttonRenderCmp->position = button->GetPosition();
                        button->AddComponent(buttonRenderCmp);
                    } // if createButtonRenderCmp == success
                    const auto& createLabelRenderCmp{
                        engine.GetRenderSystem().CreateRenderComponent(layerID, label->GetID(), label->GetFont(), label->GetFontProperties(), label->GetContents())
                    };
                    if(createLabelRenderCmp.has_value()) {
                        engine::RenderComponent* labelRenderCmp{createLabelRenderCmp.value()};
                        labelRenderCmp->scale = label->GetScale();
                        labelRenderCmp->position = label->GetPosition();
                        label->AddComponent(labelRenderCmp);
                    } // if createLabelRenderCmp == success
                } // for each MenuItem in Menu
            } // else if Menu
        } // for each child in Scene
    } // for each layer in layoutConfig
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateDecorations(ui::Scene& scene, const std::vector<ui::DecorationConfig>& decorationConfigList) {
    const auto& windowSize{engine.GetWindow().getSize()};
    for(const auto& decorationConfig: decorationConfigList) {
        // Test length of decorationID to determine if this config is valid
        if(!decorationConfig.id.empty()) {
            // Load Texture from Scene cache
            sf::Texture* texture{scene.GetCachedTexture(decorationConfig.textureProperties.id)};
            if(!texture) {
                const std::string failMsg{"NullPtr Error: Failed to retrieve Texture from Scene cache."};
                return engine::Error{engine::Error::Signal::NullPtr, failMsg};
            } // if texture == nullptr

            // Create the Decoration
            const auto& createDecoration{ui::Decoration::Create(decorationConfig.id, texture, &scene)};
            if(createDecoration.has_value()) {
                ui::Decoration* decoration{createDecoration.value()};
                decoration->SetScale(sf::Vector2f{decorationConfig.textureProperties.scale, decorationConfig.textureProperties.scale});
                // Create a temporary Sprite to determine the Decoration's size
                sf::Sprite tempSprite{*texture};
                tempSprite.setScale(decoration->GetScale());
                decoration->SetSize(sf::Vector2u{
                    static_cast<unsigned int>(tempSprite.getGlobalBounds().size.x),
                    static_cast<unsigned int>(tempSprite.getGlobalBounds().size.y)});
                const sf::Vector2f position{
                    static_cast<float>(windowSize.x * decorationConfig.relativePosition.x) / 100.0f,
                    static_cast<float>(windowSize.y * decorationConfig.relativePosition.y) / 100.0f
                };
                decoration->SetPosition(position);
            } // if createDecoration == success
        } // if decorationConfig has valid ID
    } // for each decorationConfig in layerConfig.decorations
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateTitle(ui::Scene& scene, const ui::LabelConfig& titleConfig) {
    const auto& windowSize{engine.GetWindow().getSize()};
    // Test length of labelID to determine if this config is valid
    if(!titleConfig.id.empty()) {
        // Load Font & FontProperties from Scene cache
        sf::Font* font{scene.GetCachedFont(titleConfig.fontProperties.id)};
        const FontProperties& fontProperties{scene.GetCachedFontProperties(titleConfig.fontProperties.id)};
        if(!font) {
            const std::string failMsg{"NullPtr Error: Failed to retrieve Font from Scene cache."};
            return engine::Error{engine::Error::Signal::NullPtr, failMsg};
        } // if font == null
        if(fontProperties.id != titleConfig.fontProperties.id) {
            const std::string failMsg{"Invalid configuration: fontID does not match expected value (\"" + titleConfig.fontProperties.id + "\")"};
            return engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg};
        } // if fontProperties.id does not match correct ResourceID

        // Create the Title
        const auto& createTitle{ui::Label::Create(titleConfig.id, font, fontProperties, &scene)};
        if(createTitle.has_value()) {
            ui::Label* label{createTitle.value()};
            label->SetContents(titleConfig.contents);
            label->SetScale(sf::Vector2f{label->GetFontProperties().scale, label->GetFontProperties().scale});
            // Create a temporary Text to determine the Label's size
            sf::Text tempText{*font, titleConfig.contents, fontProperties.characterSize};
            label->SetSize(sf::Vector2u{
                static_cast<unsigned int>(tempText.getGlobalBounds().size.x),
                static_cast<unsigned int>(tempText.getGlobalBounds().size.y)});
            const sf::Vector2f position{
                static_cast<float>(windowSize.x * titleConfig.relativePosition.x) / 100.0f - label->GetSize().x / 2.0f,
                static_cast<float>(windowSize.y * titleConfig.relativePosition.y) / 100.0f
            };
            label->SetPosition(position);
        } // if createTitle == success
    } // if title has valid ID
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateMenu(ui::Scene& scene, const ui::MenuConfig& menuConfig) {
    const auto& windowSize{engine.GetWindow().getSize()};
    // Test length of menuID to determine if this config is valid
    if(!menuConfig.id.empty()) {
        // Load Font & FontProperties from Scene cache
        sf::Font* font{scene.GetCachedFont(menuConfig.fontProperties.id)};
        const auto& fontProperties{scene.GetCachedFontProperties(menuConfig.fontProperties.id)};

        // Create a list of all Button Textures
        std::vector<sf::Texture*> buttonTextures;
        for(const auto& textureProperties: menuConfig.buttonConfig.textureProperties | std::views::values) {
            const ResourceID textureID{textureProperties.id + "Texture"};
            sf::Texture* texture{scene.GetCachedTexture(textureID)};
            if(texture) {
                buttonTextures.push_back(texture);
            } // if texture
        } // for each texture in buttonConfig

        // Create 3 lists: names, contents, and onSelection Events for all MenuItems
        std::vector<std::string> nameList;
        std::vector<std::string> contentsList;
        std::vector<engine::Event::Type> eventList;
        for(const auto& itemConfig: menuConfig.menuItemConfigs) {
            nameList.push_back(itemConfig.id);
            contentsList.push_back(itemConfig.labelConfig.contents);
            eventList.push_back(itemConfig.onSelect);
        } // for each itemConfig in layerConfig.menu.menuItemConfigs

        // Create the Menu using the above lists
        const auto& createMenu{
            ui::Menu::Create(
                menuConfig.id,
                font,
                fontProperties,
                buttonTextures,
                nameList,
                contentsList,
                eventList,
                &scene)
        };
        if(createMenu.has_value()) {
            ui::Menu* menu{createMenu.value()};
            // For each MenuItem, configure all Buttons and Labels
            for(int index = 0; index < menu->GetNumMenuItems(); ++index) {
                ui::MenuItem* item{menu->GetMenuItemByIndex(index)};
                ui::Label* label{item->GetLabel()};
                ui::Button* button{item->GetButton()};
                ResourceID textureID{menuConfig.buttonConfig.textureProperties.at(static_cast<int>(button->GetStatus())).second.id};
                textureID.append("Texture");

                // Load Texture & TextureProperties from Scene cache
                sf::Texture* texture{scene.GetCachedTexture(textureID)};
                const TextureProperties& textureProperties{scene.GetCachedTextureProperties(textureID)};
                button->SetScale(sf::Vector2f{textureProperties.scale, textureProperties.scale});

                // Create a temporary Sprite to determine Button's size
                sf::Sprite tempSprite{*texture};
                button->SetSize(sf::Vector2u{
                    static_cast<unsigned int>(tempSprite.getGlobalBounds().size.x),
                    static_cast<unsigned int>(tempSprite.getGlobalBounds().size.y)});
                button->SetPosition(sf::Vector2f{
                    static_cast<float>(windowSize.x * menuConfig.relativePosition.x) / 100.0f - button->GetSize().x / 2.0f,
                    static_cast<float>(windowSize.y * menuConfig.relativePosition.y) / 100.0f + (index * 1.5f * button->GetSize().y)
                });

                // Create a temporary Text to determine Label's size
                label->SetScale(sf::Vector2f{fontProperties.scale, fontProperties.scale});
                sf::Text tempText{*font, std::string{item->GetLabel()->GetContents()}, fontProperties.characterSize};
                tempText.setScale(label->GetScale());
                label->SetSize(sf::Vector2u{
                    static_cast<unsigned int>(tempText.getGlobalBounds().size.x),
                    static_cast<unsigned int>(tempText.getGlobalBounds().size.y)
                });
                label->SetPosition(sf::Vector2f{
                    button->GetPosition().x + button->GetSize().x / 2.0f - label->GetSize().x / 2.0f,
                    button->GetPosition().y + button->GetSize().y / 2.0f - label->GetSize().y / 3.0f
                });
            } // for each MenuItem in Menu
            // Create MenuController
            sceneHierarchy.menuController = std::make_unique<ui::MenuController>(menu, engine.GetEventSystem());
            engine.GetInputSystem().AddKeyListener(sceneHierarchy.menuController.get());
            engine.GetInputSystem().AddMouseListener(sceneHierarchy.menuController.get());
        } // if createMenu == success
    } // if Menu has valid ID
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheFonts(ui::Scene& scene, const std::vector<FontProperties>& fontPropertiesList) {
    for(const auto& fontProperties: fontPropertiesList) {
        // Create ResourceHandle for the Font
        const auto& createHandle{
            engine.GetResourceManager().CreateResourceHandle(fontProperties.id, ResourceHandle::Type::Font, fontProperties.path)
        };
        if(createHandle.has_value()) {
            // Load the Font
            const auto& loadFont{engine.GetResourceManager().GetFont(fontProperties.id)};
            // Cache the Font and its FontProperties in the Scene
            scene.CacheFont(fontProperties.id, loadFont.value());
            scene.CacheFontProperties(fontProperties.id, fontProperties);
        } // createHandle == success
    } // for each font in config.fonts
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheTextures(ui::Scene& scene, const std::vector<TextureProperties>& texturePropertiesList) {
    for(const auto& textureProperties: texturePropertiesList) {
        // Create ResourceHandle for the Texture
        const auto& createHandle{
            engine.GetResourceManager().CreateResourceHandle(textureProperties.id, ResourceHandle::Type::Texture, textureProperties.path)
        };
        if(createHandle.has_value()) {
            // Load the Texture
            const auto& loadTexture{engine.GetResourceManager().GetTexture(textureProperties)};
            // Cache the Texture and its TextureProperties in the Scene
            scene.CacheTexture(textureProperties.id, loadTexture.value());
            scene.CacheTextureProperties(textureProperties.id, textureProperties);
        } // createHandle == success
    } // for each texture in config.textures
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheRepeatingTextures(ui::Scene& scene, const std::vector<RepeatingTextureProperties>& texturePropertiesList) {
    for(const auto& textureProperties: texturePropertiesList) {

        // Create ResourceHandles for both inputID and outputID
        const auto& createInputHandle{
            engine.GetResourceManager().CreateResourceHandle(textureProperties.inputID, ResourceHandle::Type::Texture, textureProperties.path)
        };
        const auto& createOutputHandle{
            engine.GetResourceManager().CreateResourceHandle(textureProperties.outputID, ResourceHandle::Type::Texture, textureProperties.path)
        };

        if(createInputHandle.has_value() && createOutputHandle.has_value()) {
            // Load the Texture
            const auto& loadTexture(engine.GetResourceManager().GetTexture(textureProperties));
            // Cache the Texture and its TextureProperties in the Scene
            scene.CacheTexture(textureProperties.outputID, loadTexture.value());
            scene.CacheTextureProperties(textureProperties.outputID, TextureProperties{
                textureProperties.outputID,
                textureProperties.path,
                textureProperties.scale,
                textureProperties.inputRect
            });
        } // createHandle == success
    } // for each repeatingTexture in config.repeatingTextures
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheSegmentedTextures(ui::Scene& scene, const std::vector<SegmentedTextureProperties>& texturePropertiesList) {
    for(const auto& textureProperties: texturePropertiesList) {
        // Create ResourceHandle for the Texture
        const auto& createHandle{
            engine.GetResourceManager().CreateResourceHandle(textureProperties.id, ResourceHandle::Type::Texture, textureProperties.path)
        };

        std::vector<ResourceID> segmentIDList;
        if(createHandle.has_value()) {
            // Iterate through each Segment of the SegmentedTexture
            for(const auto& segmentProperties : textureProperties.segments) {
                // Create ResourceHandle for the Segment
                const auto& createHandle{
                    engine.GetResourceManager().CreateResourceHandle(segmentProperties.id, ResourceHandle::Type::Texture, segmentProperties.path)
                };
                if(createHandle.has_value()) {
                    // Load the Segment
                    const auto& loadTexture{engine.GetResourceManager().GetTexture(segmentProperties)};
                    if(loadTexture.has_value()) {
                        // Add its ID to segmentIDList
                        segmentIDList.emplace_back(segmentProperties.id);
                    }
                } // if createHandle == success
            } // for each segment in segmentedTexture

            // Each Segment has now been loaded -- time to splice them together!
            const auto& createTexture{engine.GetResourceManager().CreateSplicedTexture(textureProperties.id, segmentIDList)};
            if(createTexture.has_value()) {
                // Cache the splicedTexture
                scene.CacheTexture(textureProperties.id, createTexture.value());
                scene.CacheTextureProperties(textureProperties.id, TextureProperties{
                    textureProperties.id,
                    textureProperties.path,
                    textureProperties.scale,
                    sf::IntRect{{0, 0}, {0, 0}}
                });
            }
        } // createHandle == success
    } // for each segmentedTexture in config.segmentedTextures
    return std::nullopt;
}
