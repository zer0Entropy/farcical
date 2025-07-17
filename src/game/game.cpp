//
// Created by dgmuller on 6/7/25.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include "../../include/game/game.hpp"
#include "../../include/ui/menu.hpp"
#include "../../include/ui/decoration.hpp"

farcical::game::GameController::GameController(Game& game): EventHandler(),
                                                            game{game} {
}

void farcical::game::GameController::HandleEvent(const engine::Event& event) {
    ResourceManager& resourceManager{game.GetEngine().GetResourceManager()};
    if(event.type == engine::Event::Type::QuitGame) {
        game.Stop();
    } // if event.type == QuitGame
    else if(event.type == engine::Event::Type::CreateScene) {
        const engine::EntityID sceneID{std::any_cast<std::string>(event.args.at(0))};

        // Request ResourceParameters for this Scene from Game
        const auto& findSceneResource{game.FindSceneResource(sceneID)};
        if(findSceneResource.has_value()) {
            const ResourceParameters& params{findSceneResource.value()};

            // If a ResourceHandle has already been created for this document, retrieve it
            const ResourceHandle* documentHandle{
                resourceManager.GetResourceHandle(params.first)
            };

            // If no such ResourceHandle exists, create it
            if(!documentHandle) {
                const auto& createDocumentHandle{
                    resourceManager.CreateResourceHandle(
                        params.first,
                        ResourceHandle::Type::JSONDocument,
                        params.second)
                };
                if(createDocumentHandle.has_value()) {
                    documentHandle = createDocumentHandle.value();
                } // if createDocumentHandle == success
            } // if !documentHandle

            // Load the document, then use the data it contains to create the scene
            const auto& requestJSONDoc{resourceManager.GetJSONDoc(documentHandle->id)};
            if(requestJSONDoc.has_value()) {
                const auto& sceneJSON{requestJSONDoc.value()};
                const auto& loadSceneConfig{ui::LoadSceneConfig(*sceneJSON)};
                if(loadSceneConfig.has_value()) {
                    auto sceneConfig{loadSceneConfig.value()};
                    ui::SceneHierarchy& sceneHierarchy{game.GetSceneHierarchy()};
                    const auto& createScene{game.CreateScene(sceneConfig, sceneHierarchy.currentScene)};
                    // Once the Scene has been created, set the hierarchy's currentScene to this one
                    ui::Scene* scene{createScene.value().get()};
                    sceneHierarchy.currentScene = scene;
                    const auto& startScene{game.StartScene(scene, sceneConfig)};
                } // if loadScene == success
            } // if requestJSONDoc == success

        } // if findSceneResource == success
    } // else if event.type == CreateScene
    else if(event.type == engine::Event::Type::DestroyScene) {
    } // else if event.type == DestroyScene
}

farcical::game::Game::Game(engine::Engine& engine): status{Status::Uninitialized},
                                                    engine{engine},
                                                    controller{*this} {
}

farcical::game::Game::Status farcical::game::Game::GetStatus() const {
    return status;
}

farcical::engine::Engine& farcical::game::Game::GetEngine() const {
    return engine;
}

farcical::ui::SceneHierarchy& farcical::game::Game::GetSceneHierarchy() const {
    return const_cast<ui::SceneHierarchy&>(sceneHierarchy);
}

std::optional<farcical::engine::Error> farcical::game::Game::Init(const ResourceList& sceneResourceList) {
    if(status != Status::Uninitialized) {
        return std::nullopt;
    }

    // Subscribe GameController to QuitGame, CreateScene, DestroyScene Events
    engine.GetEventSystem().RegisterHandler(engine::Event::Type::QuitGame, &controller);
    engine.GetEventSystem().RegisterHandler(engine::Event::Type::CreateScene, &controller);
    engine.GetEventSystem().RegisterHandler(engine::Event::Type::DestroyScene, &controller);

    // Copy all SceneResources on the list to a local cache
    for(const auto& resource: sceneResourceList) {
        sceneResources.insert(resource);
    } // for each Resource in SceneResourceList

    // Take the first SceneResource on the list and create a Scene from that.
    // This Scene will be the root of our SceneHierarchy.
    const auto& firstSceneResource{sceneResourceList[0]};
    engine::EntityID sceneID{firstSceneResource.first};
    ResourceID configDocumentID{firstSceneResource.second.first};
    const std::string configDocumentPath{firstSceneResource.second.second};

    // If a ResourceHandle has already been created for this document, retrieve it
    const ResourceHandle* documentHandle{
        engine.GetResourceManager().GetResourceHandle(configDocumentID)
    };
    // If no such ResourceHandle exists, create it
    if(!documentHandle) {
        const auto& createDocumentHandle{
            engine.GetResourceManager().CreateResourceHandle(
                configDocumentID,
                ResourceHandle::Type::JSONDocument,
                configDocumentPath)
        };
        if(createDocumentHandle.has_value()) {
            documentHandle = createDocumentHandle.value();
        } // if createDocumentHandle == success
    } // if !documentHandle

    // Load the document, then use the data it contains to create the scene
    const auto& requestJSONDoc{engine.GetResourceManager().GetJSONDoc(documentHandle->id)};
    if(requestJSONDoc.has_value()) {
        const auto& sceneJSON{requestJSONDoc.value()};
        auto loadSceneConfig{ui::LoadSceneConfig(*sceneJSON)};
        if(loadSceneConfig.has_value()) {
            auto& sceneConfig{loadSceneConfig.value()};
            auto createScene{CreateScene(sceneConfig, nullptr)};
            sceneHierarchy.root = std::move(createScene.value());
            ui::Scene* scene{sceneHierarchy.root.get()};
            sceneHierarchy.currentScene = scene;
            const auto& startScene{StartScene(scene, sceneConfig)};
            if(startScene.has_value()) {
                return startScene.value();
            }
            status = Status::IsRunning;
        } // if loadScene == success
    } // if requestJSONDoc == success

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::Update() {
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::Stop() {
    if(status == Status::IsRunning) {
        status = Status::StoppedSuccessfully;
    }
    return std::nullopt;
}

std::unique_ptr<farcical::game::World> farcical::game::Game::CreateWorld() {
    return std::make_unique<World>();
}

std::unique_ptr<farcical::game::Player> farcical::game::Game::CreatePlayer() {
    return std::make_unique<Player>();
}

std::expected<std::unique_ptr<farcical::ui::Scene>, farcical::engine::Error> farcical::game::Game::CreateScene(
    ui::SceneConfig config, ui::Scene* parent) {
    std::unique_ptr<ui::Scene> scene{std::make_unique<ui::Scene>(config.id, parent, sceneHierarchy)};
    sceneHierarchy.sceneConfigs.insert(std::make_pair(config.id, config));
    return std::move(scene);
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateSceneLayout(
    ui::Scene& scene, const ui::LayoutConfig& layoutConfig) {
    // Create the RenderContext that will encapsulate our RenderComponents
    const auto& createRenderContext{
        engine.GetRenderSystem().CreateRenderContext(scene.GetID())
    };
    if(!createRenderContext.has_value()) {
        return createRenderContext.error();
    } // if createRenderContext failed, return Error
    engine::RenderContext& context{*createRenderContext.value()};

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
                    engine.GetRenderSystem().CreateRenderComponent(
                        layerID,
                        scene.GetID(),
                        decoration->GetID(),
                        decoration->GetTexture())
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
                    engine.GetRenderSystem().CreateRenderComponent(
                        layerID,
                        scene.GetID(),
                        label->GetID(),
                        label->GetFont(),
                        label->GetFontProperties(),
                        label->GetContents())
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
                        engine.GetRenderSystem().CreateRenderComponent(
                            layerID,
                            scene.GetID(),
                            button->GetID(),
                            button->GetTexture())
                    };
                    if(createButtonRenderCmp.has_value()) {
                        engine::RenderComponent* buttonRenderCmp{createButtonRenderCmp.value()};
                        buttonRenderCmp->scale = button->GetScale();
                        buttonRenderCmp->position = button->GetPosition();
                        button->AddComponent(buttonRenderCmp);
                    } // if createButtonRenderCmp == success
                    const auto& createLabelRenderCmp{
                        engine.GetRenderSystem().CreateRenderComponent(
                            layerID,
                            scene.GetID(),
                            label->GetID(),
                            label->GetFont(),
                            label->GetFontProperties(),
                            label->GetContents())
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

std::optional<farcical::engine::Error> farcical::game::Game::CreateDecorations(
    ui::Scene& scene, const std::vector<ui::DecorationConfig>& decorationConfigList) {
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
                decoration->SetScale(sf::Vector2f{
                    decorationConfig.textureProperties.scale, decorationConfig.textureProperties.scale
                });
                // Create a temporary Sprite to determine the Decoration's size
                sf::Sprite tempSprite{*texture};
                tempSprite.setScale(decoration->GetScale());
                decoration->SetSize(sf::Vector2u{
                    static_cast<unsigned int>(tempSprite.getGlobalBounds().size.x),
                    static_cast<unsigned int>(tempSprite.getGlobalBounds().size.y)
                });
                if(decorationConfig.relativePosition.x == 0 && decorationConfig.relativePosition.y == 0) {
                    decoration->SetPosition(sf::Vector2f{0.0f, 0.0f});
                } else {
                    const sf::Vector2f position{
                        static_cast<float>(windowSize.x * decorationConfig.relativePosition.x) / 100.0f - static_cast<
                            float>(decoration->GetSize().x) / 2.0f,
                        static_cast<float>(windowSize.y * decorationConfig.relativePosition.y) / 100.0f - static_cast<
                            float>(decoration->GetSize().y) / 2.0f
                    };
                    decoration->SetPosition(position);
                }
            } // if createDecoration == success
        } // if decorationConfig has valid ID
    } // for each decorationConfig in layerConfig.decorations
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateTitle(
    ui::Scene& scene, const ui::LabelConfig& titleConfig) {
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
            const std::string failMsg{
                "Invalid configuration: fontID does not match expected value (\"" + titleConfig.fontProperties.id +
                "\")"
            };
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
                static_cast<unsigned int>(tempText.getGlobalBounds().size.y)
            });
            const sf::Vector2f position{
                static_cast<float>(windowSize.x * titleConfig.relativePosition.x) / 100.0f - label->GetSize().x / 2.0f,
                static_cast<float>(windowSize.y * titleConfig.relativePosition.y) / 100.0f
            };
            label->SetPosition(position);
        } // if createTitle == success
    } // if title has valid ID
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateMenu(
    ui::Scene& scene, const ui::MenuConfig& menuConfig) {
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
        std::vector<engine::Event::Type> eventTypeList;
        std::vector<std::vector<std::any>> eventArgsList;
        for(const auto& itemConfig: menuConfig.menuItemConfigs) {
            nameList.push_back(itemConfig.id);
            contentsList.push_back(itemConfig.labelConfig.contents);
            eventTypeList.push_back(itemConfig.activationEventType);
            eventArgsList.push_back(itemConfig.activationEventArgs);
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
                eventTypeList,
                eventArgsList,
                &scene)
        };
        if(createMenu.has_value()) {
            ui::Menu* menu{createMenu.value()};
            // For each MenuItem, configure all Buttons and Labels
            for(int index = 0; index < menu->GetNumMenuItems(); ++index) {
                ui::MenuItem* item{menu->GetMenuItemByIndex(index)};
                ui::Label* label{item->GetLabel()};
                ui::Button* button{item->GetButton()};
                ResourceID textureID{
                    menuConfig.buttonConfig.textureProperties.at(static_cast<int>(button->GetStatus())).second.id
                };
                textureID.append("Texture");

                // Load Texture & TextureProperties from Scene cache
                sf::Texture* texture{scene.GetCachedTexture(textureID)};
                const TextureProperties& textureProperties{scene.GetCachedTextureProperties(textureID)};
                button->SetScale(sf::Vector2f{textureProperties.scale, textureProperties.scale});

                // Create a temporary Sprite to determine Button's size
                sf::Sprite tempSprite{*texture};
                button->SetSize(sf::Vector2u{
                    static_cast<unsigned int>(tempSprite.getGlobalBounds().size.x),
                    static_cast<unsigned int>(tempSprite.getGlobalBounds().size.y)
                });
                button->SetPosition(sf::Vector2f{
                    static_cast<float>(windowSize.x * menuConfig.relativePosition.x) / 100.0f - button->GetSize().x /
                    2.0f,
                    static_cast<float>(windowSize.y * menuConfig.relativePosition.y) / 100.0f + (
                        index * 1.5f * button->GetSize().y)
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

std::optional<farcical::engine::Error> farcical::game::Game::CacheFonts(
    ui::Scene& scene, const std::vector<FontProperties>& fontPropertiesList) {
    for(const auto& fontProperties: fontPropertiesList) {
        // Create ResourceHandle for the Font
        const auto& createHandle{
            engine.GetResourceManager().CreateResourceHandle(fontProperties.id, ResourceHandle::Type::Font,
                                                             fontProperties.path)
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

std::optional<farcical::engine::Error> farcical::game::Game::CacheTextures(
    ui::Scene& scene, const std::vector<TextureProperties>& texturePropertiesList) {
    for(const auto& textureProperties: texturePropertiesList) {
        // Create ResourceHandle for the Texture
        const auto& createHandle{
            engine.GetResourceManager().CreateResourceHandle(textureProperties.id, ResourceHandle::Type::Texture,
                                                             textureProperties.path)
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

std::optional<farcical::engine::Error> farcical::game::Game::CacheRepeatingTextures(
    ui::Scene& scene, const std::vector<RepeatingTextureProperties>& texturePropertiesList) {
    for(const auto& textureProperties: texturePropertiesList) {
        // Create ResourceHandles for both inputID and outputID
        const auto& createInputHandle{
            engine.GetResourceManager().CreateResourceHandle(textureProperties.inputID, ResourceHandle::Type::Texture,
                                                             textureProperties.path)
        };
        const auto& createOutputHandle{
            engine.GetResourceManager().CreateResourceHandle(textureProperties.outputID, ResourceHandle::Type::Texture,
                                                             textureProperties.path)
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

std::optional<farcical::engine::Error> farcical::game::Game::CacheSegmentedTextures(
    ui::Scene& scene, const std::vector<SegmentedTextureProperties>& texturePropertiesList) {
    for(const auto& textureProperties: texturePropertiesList) {
        // Create ResourceHandle for the Texture
        const auto& createHandle{
            engine.GetResourceManager().CreateResourceHandle(textureProperties.id, ResourceHandle::Type::Texture,
                                                             textureProperties.path)
        };

        if(createHandle.has_value()) {
            std::vector<ResourceID> segmentIDList;
            // Iterate through each Segment of the SegmentedTexture
            for(const auto& segmentProperties: textureProperties.segments) {
                // Create ResourceHandle for the Segment
                const auto& createHandle{
                    engine.GetResourceManager().CreateResourceHandle(segmentProperties.id,
                                                                     ResourceHandle::Type::Texture,
                                                                     segmentProperties.path)
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
            const auto& createTexture{
                engine.GetResourceManager().CreateSplicedTexture(textureProperties.id, segmentIDList)
            };
            if(createTexture.has_value()) {
                // Cache the splicedTexture
                scene.CacheTexture(textureProperties.id, createTexture.value());
                scene.CacheTextureProperties(
                    textureProperties.id, TextureProperties{
                        textureProperties.id,
                        textureProperties.path,
                        textureProperties.scale,
                        sf::IntRect{{0, 0}, {0, 0}}
                    });
            } // if createTexture == success
        } // createHandle == success
    } // for each segmentedTexture in config.segmentedTextures
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheBorderTexture(ui::Scene& scene,
    const BorderTextureProperties& borderProperties) {
    if(borderProperties.id.empty()) {
        return std::nullopt;
    }
    ResourceManager& resourceManager{engine.GetResourceManager()};

    // Create ResourceHandle for the Texture
    const auto& createHandle{
        resourceManager.CreateResourceHandle(borderProperties.id, ResourceHandle::Type::Texture, borderProperties.path)
    };
    if(createHandle.has_value()) {
        std::vector<ResourceID> cornerTextureIDs;
        std::vector<ResourceID> edgeTextureIDs;

        // Corners
        for(const auto& corner: borderProperties.cornerTextures) {
            // Create ResourceHandle for each corner Texture
            const auto& createCornerHandle{
                resourceManager.CreateResourceHandle(corner.id, ResourceHandle::Type::Texture, corner.path)
            };
            if(createCornerHandle.has_value()) {
                const auto& loadCornerTexture{
                    resourceManager.GetTexture(corner)
                };
                if(loadCornerTexture.has_value()) {
                    scene.CacheTexture(corner.id, loadCornerTexture.value());
                    scene.CacheTextureProperties(corner.id, corner);
                    cornerTextureIDs.emplace_back(corner.id);
                } // if loadCornerTexture == success
            } // if createCornerHandle == success
        } // for each corner

        // Edges
        for(const auto& edge: borderProperties.edgeTextures) {
            // Create ResourceHandle for each edge Texture
            const auto& createEdgeHandle{
                resourceManager.CreateResourceHandle(edge.id, ResourceHandle::Type::Texture, edge.path)
            };
            if(createEdgeHandle.has_value()) {
                const auto& loadEdgeTexture{
                    resourceManager.GetTexture(edge)
                };
                if(loadEdgeTexture.has_value()) {
                    scene.CacheTexture(edge.id, loadEdgeTexture.value());
                    scene.CacheTextureProperties(edge.id, edge);
                    edgeTextureIDs.emplace_back(edge.id);
                } // if loadEdgeTexture == success
            } // if createEdgeHandle == success
        } // for each edge

        // Center
        const auto& createCenterHandle{
            resourceManager.CreateResourceHandle(
                borderProperties.centerTexture.id,
                ResourceHandle::Type::Texture,
                borderProperties.centerTexture.path)
        };
        if(createCenterHandle.has_value()) {
            const auto& loadCenterTexture{
                resourceManager.GetTexture(borderProperties.centerTexture)
            };
            if(loadCenterTexture.has_value()) {
                scene.CacheTexture(borderProperties.centerTexture.id, loadCenterTexture.value());
                scene.CacheTextureProperties(borderProperties.centerTexture.id, borderProperties.centerTexture);
            } // if loadCenterTexture == success
        } // if createCenterHandle == success

        const auto& createBorderTexture{
            resourceManager.CreateBorderTexture(
                borderProperties.id,
                borderProperties.outputSize,
                cornerTextureIDs,
                edgeTextureIDs,
                borderProperties.centerTexture.id)
        };
        if(!createBorderTexture.has_value()) {
            const std::string failMsg{"Unexpected nullptr: Failed to create borderTexture."};
            return engine::Error{engine::Error::Signal::NullPtr, failMsg};
        }
        scene.CacheTexture(borderProperties.id, createBorderTexture.value());
        scene.CacheTextureProperties(borderProperties.id, TextureProperties{
                                         borderProperties.id,
                                         borderProperties.path,
                                         borderProperties.scale,
                                         sf::IntRect{{0, 0}, {0, 0}}
                                     });
    } // if createHandle == success

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::StartScene(ui::Scene* scene, ui::SceneConfig& config) {
    const sf::Vector2u& windowSize{engine.GetConfig().windowProperties.sizeInPixels};

    /***************           CACHE RESOURCES         ***************/
    /*  FONTS  */
    const auto& cacheFontsResult{
        CacheFonts(*scene, config.fonts)
    };
    if(cacheFontsResult.has_value()) {
        return cacheFontsResult.value();
    } // if cacheFontsResult == failure
    /*  TEXTURES  */
    const auto& cacheTexturesResult{
        CacheTextures(*scene, config.textures)
    };
    if(cacheTexturesResult.has_value()) {
        return cacheTexturesResult;
    } // if cacheTexturesResult == failure
    /*  REPEATING TEXTURES  */
    const auto& cacheRepeatingResult{
        CacheRepeatingTextures(*scene, config.repeatingTextures)
    };
    if(cacheRepeatingResult.has_value()) {
        return cacheRepeatingResult;
    } // if cacheRepeatingResult == failure
    /*  SEGMENTED TEXTURES  */
    const auto& cacheSegmentedResult{
        CacheSegmentedTextures(*scene, config.segmentedTextures)
    };
    if(cacheSegmentedResult.has_value()) {
        return cacheSegmentedResult;
    } // if cacheSegmentedResult == failure
    /* BORDER TEXTURE */
    config.borderTexture.outputSize = {
        static_cast<unsigned int>(config.borderTexture.percentSize.x * windowSize.x / 100 / config.borderTexture.scale),
        static_cast<unsigned int>(config.borderTexture.percentSize.y * windowSize.y / 100 / config.borderTexture.scale)
    };
    const auto& cacheBorderResult{
        CacheBorderTexture(*scene, config.borderTexture)
    };
    if(cacheBorderResult.has_value()) {
        return cacheBorderResult.value();
    } // if cacheBorderResult == failure
    /*****************************************************************/

    /*  CREATE THE LAYOUT   */
    const auto& createLayoutResult{
        CreateSceneLayout(*scene, config.layout)
    };
    if(createLayoutResult.has_value()) {
        return createLayoutResult.value();
    } // if createLayoutResult == failure

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::StopScene(ui::Scene* scene) {
    return std::nullopt;
}

std::expected<farcical::ResourceParameters, farcical::engine::Error> farcical::game::Game::FindSceneResource(
    engine::EntityID sceneID) const {
    const auto& findResource{sceneResources.find(sceneID)};
    if(findResource != sceneResources.end()) {
        return findResource->second;
    } // if SceneResource found
    const std::string failMsg{"Resource not found: " + sceneID + "."};
    return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
}
