//
// Created by dgmuller on 6/7/25.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../../include/game/game.hpp"
#include "../../include/ui/menu.hpp"
#include "../../include/ui/decoration.hpp"
#include "../../include/engine/log.hpp"
#include "../../include/ui/factory.hpp"

farcical::game::GameController::GameController(Game& game):
    EventHandler(),
    LogInterface(game.GetEngine().GetLogSystem()), game{game} {
}

void farcical::game::GameController::HandleEvent(const engine::Event& event) {
    ResourceManager& resourceManager{game.GetEngine().GetResourceManager()};
    if(event.type == engine::Event::Type::QuitGame) {
        WriteToLog("GameController received 'QuitGame' event.");
        game.Stop();
    } // if event.type == QuitGame
    else if(event.type == engine::Event::Type::ApplyEngineConfig) {
        WriteToLog("GameController received 'ApplyEngineConfig' event.");
        const auto& createConfig{game.GetEngine().CreateConfig()};
        if(createConfig.has_value()) {
            const auto& applyConfig{game.GetEngine().ApplyConfig(createConfig.value())};
        } // if
    } // else if event.type == ApplyEngineConfig
    else if(event.type == engine::Event::Type::CreateScene) {
        std::string logMsg{"GameController received 'CreateScene' event (args=\""};
        for(const auto& arg: event.args) {
            if(arg.type() == typeid(std::string)) {
                const std::string argString{std::any_cast<std::string>(arg)};
                logMsg.append(argString);
            } // string
        } // for each arg
        logMsg.append("\").");
        WriteToLog(logMsg);
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
                    // The previous Scene needs to be stopped before we start this one
                    const auto& stopSceneResult{game.StopCurrentScene()};
                    // Now set currentScene to the new Scene and start it
                    if(!stopSceneResult.has_value()) {
                        ui::SceneConfig& sceneConfig{const_cast<ui::SceneConfig&>(loadSceneConfig.value())};
                        const auto& createScene{game.CreateScene(sceneConfig)};
                        const auto& startSceneResult{game.StartCurrentScene(sceneConfig)};
                        if(startSceneResult.has_value()) {
                            game.Stop();
                        } // if startSceneResult == failure
                    } // if stopSceneResult == success
                } // if loadScene == success
            } // if requestJSONDoc == success

        } // if findSceneResource == success
    } // else if event.type == CreateScene
    else if(event.type == engine::Event::Type::DestroyScene) {
    } // else if event.type == DestroyScene
}

farcical::game::Game::Game(engine::Engine& engine):
    status{Status::Uninitialized},
    engine{engine},
    controller{nullptr} {
}

farcical::game::Game::Status farcical::game::Game::GetStatus() const {
    return status;
}

farcical::engine::Engine& farcical::game::Game::GetEngine() const {
    return engine;
}

std::optional<farcical::engine::Error> farcical::game::Game::Init(const ResourceList& sceneResourceList) {
    engine::LogSystem& logSystem{engine.GetLogSystem()};
    if(status != Status::Uninitialized) {
        if(status == Status::IsRunning) {
            logSystem.AddMessage("Error: Game.Init() called, but Game is already initialized!");
        } else if(status == Status::Error) {
            logSystem.AddMessage("Unknown error occurred in game.Init() [game.status == Error]");
        }
        return std::nullopt;
    }
    logSystem.AddMessage("Initializing game...");
    logSystem.AddMessage("");


    logSystem.AddMessage("Creating GameController...");
    controller = std::make_unique<GameController>(*this);

    // Subscribe GameController to QuitGame, CreateScene, DestroyScene Events
    logSystem.AddMessage("Registering EventHandlers...");
    engine.GetEventSystem().RegisterHandler(engine::Event::Type::QuitGame, controller.get());
    engine.GetEventSystem().RegisterHandler(engine::Event::Type::CreateScene, controller.get());
    engine.GetEventSystem().RegisterHandler(engine::Event::Type::DestroyScene, controller.get());

    // Copy all SceneResources on the list to a local cache
    logSystem.AddMessage("Loading Scene resources...");
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
        logSystem.AddMessage("Creating JSONDocument handle...");
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
            logSystem.AddMessage("");
            logSystem.AddMessage("Creating scene...");
            auto& sceneConfig{loadSceneConfig.value()};
            auto createScene{CreateScene(sceneConfig)};
            logSystem.AddMessage("Starting scene...");
            const auto& startScene{StartCurrentScene(sceneConfig)};
            if(startScene.has_value()) {
                return startScene.value();
            }
            status = Status::IsRunning;
            logSystem.AddMessage("Game initialized.");
            logSystem.AddMessage("");
        } // if loadScene == success
    } // if requestJSONDoc == success

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::Update() {
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::Stop() {
    if(status == Status::IsRunning) {
        if(controller) {
            controller.reset();
        }
        status = Status::StoppedSuccessfully;
        engine.Stop();
    }
    return std::nullopt;
}

std::unique_ptr<farcical::game::World> farcical::game::Game::CreateWorld() {
    return std::make_unique<World>();
}

std::unique_ptr<farcical::game::Player> farcical::game::Game::CreatePlayer() {
    return std::make_unique<Player>();
}

std::expected<farcical::ui::Scene*, farcical::engine::Error> farcical::game::Game::CreateScene(ui::SceneConfig config) {
    std::unique_ptr<ui::Scene> scene{std::make_unique<ui::Scene>(config.id, *this)};
    const auto& findConfig{sceneConfigs.find(config.id)};
    if(findConfig != sceneConfigs.end()) {
        sceneConfigs.erase(findConfig);
    } // if a config for this Scene already exists, erase it
    const auto& createSceneResult{sceneConfigs.emplace(config.id, config)};
    engine.GetLogSystem().AddMessage("Scene created successfully.");
    currentScene = std::move(scene);
    return currentScene.get();
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateSceneLayout(const ui::LayoutConfig& layoutConfig) {

    // Create the RenderContext that will encapsulate our RenderComponents
    const auto& createRenderContext{
        engine.GetRenderSystem().CreateRenderContext(currentScene->GetID())
    };
    if(!createRenderContext.has_value()) {
        return createRenderContext.error();
    } // if createRenderContext failed, return Error
    engine::RenderContext& context{*createRenderContext.value()};

    for(const auto& layerConfig: layoutConfig.layers) {
        for(const auto& decorationConfig: layerConfig.decorations) {
            const auto& createDecoration{
                ui::factory::CreateDecoration(*this, decorationConfig, &currentScene->GetRootContainer())
            };
        } // for each Decoration
        const auto& createTitle{
            ui::factory::CreateTitle(*this, layerConfig.title, &currentScene->GetRootContainer())
        };
        const auto& createMenu{
            ui::factory::CreateMenu(*this, layerConfig.menu, &currentScene->GetRootContainer())
        };
        for(const auto& widget: currentScene->GetTopLevelWidgets()) {
            ui::Widget::Type widgetType{widget->GetType()};
            if(widgetType == ui::Widget::Type::Decoration) {
                ui::Decoration* decoration{dynamic_cast<ui::Decoration*>(widget)};
                const auto& createRenderCmp{
                    engine.GetRenderSystem().CreateRenderComponent(
                        layerConfig.id,
                        currentScene->GetID(),
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
                ui::Label* label{dynamic_cast<ui::Label*>(widget)};
                const auto& createRenderCmp{
                    engine.GetRenderSystem().CreateRenderComponent(
                        layerConfig.id,
                        currentScene->GetID(),
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
                ui::Menu* menu{dynamic_cast<ui::Menu*>(widget)};
                for(int index = 0; index < menu->GetNumMenuItems(); ++index) {
                    ui::MenuItem* item{menu->GetMenuItemByIndex(index)};
                    ui::Button* button{item->GetButton()};
                    ui::Label* label{item->GetLabel()};
                    const auto& createButtonRenderCmp{
                        engine.GetRenderSystem().CreateRenderComponent(
                            layerConfig.id,
                            currentScene->GetID(),
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
                            layerConfig.id,
                            currentScene->GetID(),
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

std::optional<farcical::engine::Error> farcical::game::Game::DestroySceneLayout(const ui::LayoutConfig& layoutConfig) {
    for(const auto& layerConfig: layoutConfig.layers) {
        for(const auto& decorationConfig: layerConfig.decorations) {
            ui::Decoration* decoration{dynamic_cast<ui::Decoration*>(currentScene->FindWidget(decorationConfig.id))};
            const auto& destroyDecoration{ui::factory::DestroyDecoration(*this, decoration)};
            if(destroyDecoration.has_value()) {
                return destroyDecoration.value();
            } // if destroyDecoration == failure
        } // for each Decoration

        ui::Label* title{dynamic_cast<ui::Label*>(currentScene->FindWidget(layerConfig.title.id))};
        if(title) {
            const auto& destroyTitleResult{ui::factory::DestroyTitle(*this, title)};
            if(destroyTitleResult.has_value()) {
                return destroyTitleResult.value();
            } // if destroyTitleResult == failure
        } // if title

        ui::Menu* menu{dynamic_cast<ui::Menu*>(currentScene->FindWidget(layerConfig.menu.id))};
        if(menu) {
            const auto& destroyMenuResult{
                ui::factory::DestroyMenu(*this, menu)
            };
            if(destroyMenuResult.has_value()) {
                return destroyMenuResult.value();
            } // if destroyMenuResult == failure
        } // if menu
    } // for each layer in layoutConfig

    // Destroy RenderContext
    engine.GetRenderSystem().DestroyRenderContext(currentScene->GetID());

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheFonts(const std::vector<FontProperties>& fontPropertiesList) {
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
            currentScene->CacheFont(fontProperties.id, loadFont.value());
            currentScene->CacheFontProperties(fontProperties.id, fontProperties);
        } // createHandle == success
    } // for each font in config.fonts
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheTextures(const std::vector<TextureProperties>& texturePropertiesList) {
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
            currentScene->CacheTexture(textureProperties.id, loadTexture.value());
            currentScene->CacheTextureProperties(textureProperties.id, textureProperties);
        } // createHandle == success
    } // for each texture in config.textures
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheRepeatingTextures(const std::vector<RepeatingTextureProperties>& texturePropertiesList) {
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
            currentScene->CacheTexture(textureProperties.outputID, loadTexture.value());
            currentScene->CacheTextureProperties(textureProperties.outputID, TextureProperties{
                                                     textureProperties.outputID,
                                                     textureProperties.path,
                                                     textureProperties.scale,
                                                     textureProperties.inputRect
                                                 });
        } // createHandle == success
    } // for each repeatingTexture in config.repeatingTextures
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CacheSegmentedTextures(const std::vector<SegmentedTextureProperties>& texturePropertiesList) {
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
                currentScene->CacheTexture(textureProperties.id, createTexture.value());
                currentScene->CacheTextureProperties(
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

std::optional<farcical::engine::Error> farcical::game::Game::CacheBorderTexture(const BorderTextureProperties& borderProperties) {
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
                    currentScene->CacheTexture(corner.id, loadCornerTexture.value());
                    currentScene->CacheTextureProperties(corner.id, corner);
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
                    currentScene->CacheTexture(edge.id, loadEdgeTexture.value());
                    currentScene->CacheTextureProperties(edge.id, edge);
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
                currentScene->CacheTexture(borderProperties.centerTexture.id, loadCenterTexture.value());
                currentScene->CacheTextureProperties(borderProperties.centerTexture.id, borderProperties.centerTexture);
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
        currentScene->CacheTexture(borderProperties.id, createBorderTexture.value());
        currentScene->CacheTextureProperties(borderProperties.id, TextureProperties{
                                                 borderProperties.id,
                                                 borderProperties.path,
                                                 borderProperties.scale,
                                                 sf::IntRect{{0, 0}, {0, 0}}
                                             });
    } // if createHandle == success

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::StartCurrentScene(ui::SceneConfig& config) {

    if(!currentScene) {
        return std::nullopt;
    } // if !currentScene

    const sf::Vector2u& windowSize{engine.GetConfig().windowProperties.sizeInPixels};
    /***************           CACHE RESOURCES         ***************/
    /*  FONTS  */
    const auto& cacheFontsResult{
        CacheFonts(config.fonts)
    };
    if(cacheFontsResult.has_value()) {
        return cacheFontsResult.value();
    } // if cacheFontsResult == failure
    /*  TEXTURES  */
    const auto& cacheTexturesResult{
        CacheTextures(config.textures)
    };
    if(cacheTexturesResult.has_value()) {
        return cacheTexturesResult;
    } // if cacheTexturesResult == failure
    /*  REPEATING TEXTURES  */
    const auto& cacheRepeatingResult{
        CacheRepeatingTextures(config.repeatingTextures)
    };
    if(cacheRepeatingResult.has_value()) {
        return cacheRepeatingResult;
    } // if cacheRepeatingResult == failure
    /*  SEGMENTED TEXTURES  */
    const auto& cacheSegmentedResult{
        CacheSegmentedTextures(config.segmentedTextures)
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
        CacheBorderTexture(config.borderTexture)
    };
    if(cacheBorderResult.has_value()) {
        return cacheBorderResult.value();
    } // if cacheBorderResult == failure
    /*****************************************************************/

    /*  CREATE THE LAYOUT   */
    const auto& createLayoutResult{
        CreateSceneLayout(config.layout)
    };
    if(createLayoutResult.has_value()) {
        return createLayoutResult.value();
    } // if createLayoutResult == failure

    engine.GetLogSystem().AddMessage("Scene started successfully.");
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::StopCurrentScene() {
    if(!currentScene) {
        return std::nullopt;
    } // if !currentScene
    // Request ResourceParameters for this Scene
    const auto& findSceneResource{FindSceneResource(currentScene->GetID())};
    if(findSceneResource.has_value()) {
        const ResourceParameters& params{findSceneResource.value()};
        // If a ResourceHandle has already been created for this document, retrieve it
        const ResourceHandle* documentHandle{
            engine.GetResourceManager().GetResourceHandle(params.first)
        };
        // Load the document, then use the data it contains to destroy the scene
        const auto& requestJSONDoc{engine.GetResourceManager().GetJSONDoc(documentHandle->id)};
        if(requestJSONDoc.has_value()) {
            const auto& sceneJSON{requestJSONDoc.value()};
            auto loadSceneConfig{ui::LoadSceneConfig(*sceneJSON)};
            if(loadSceneConfig.has_value()) {
                const ui::SceneConfig& sceneConfig{loadSceneConfig.value()};
                const auto destroyLayoutResult{DestroySceneLayout(sceneConfig.layout)};
                if(destroyLayoutResult.has_value()) {
                    return destroyLayoutResult;
                } // if destroyLayoutResult == failure
                // Unload all Textures and destroy their ResourceHandles
                for(const auto& texture: sceneConfig.textures) {
                    engine.GetResourceManager().DestroyResourceHandle(texture.id, ResourceHandle::Type::Texture);
                } // for each texture
                for(const auto& repeatingTexture: sceneConfig.repeatingTextures) {
                    engine.GetResourceManager().DestroyResourceHandle(repeatingTexture.inputID,
                                                                      ResourceHandle::Type::Texture);
                    engine.GetResourceManager().DestroyResourceHandle(repeatingTexture.outputID,
                                                                      ResourceHandle::Type::Texture);
                } // for each repeatingTexture
                for(const auto& segmentedTexture: sceneConfig.segmentedTextures) {
                    for(const auto& segment: segmentedTexture.segments) {
                        engine.GetResourceManager().DestroyResourceHandle(segment.id, ResourceHandle::Type::Texture);
                    }
                    engine.GetResourceManager().DestroyResourceHandle(segmentedTexture.id,
                                                                      ResourceHandle::Type::Texture);
                } // for each segmentedTexture
                if(!sceneConfig.borderTexture.id.empty()) {
                    for(const auto& corner: sceneConfig.borderTexture.cornerTextures) {
                        engine.GetResourceManager().DestroyResourceHandle(corner.id, ResourceHandle::Type::Texture);
                    } // for each corner
                    for(const auto& edge: sceneConfig.borderTexture.edgeTextures) {
                        engine.GetResourceManager().DestroyResourceHandle(edge.id, ResourceHandle::Type::Texture);
                    } // for each edge
                    engine.GetResourceManager().DestroyResourceHandle(sceneConfig.borderTexture.centerTexture.id,
                                                                      ResourceHandle::Type::Texture);
                    engine.GetResourceManager().DestroyResourceHandle(sceneConfig.borderTexture.id,
                                                                      ResourceHandle::Type::Texture);
                } // if borderTexture.id != empty
                // Unload all Fonts and destroy their ResourceHandles
                for(const auto& font: sceneConfig.fonts) {
                    engine.GetResourceManager().DestroyResourceHandle(font.id, ResourceHandle::Type::Font);
                } // for each Font in sceneConfig
            } // if loadSceneConfig == success
        } // if requestJSONDoc == success
    } // if findSceneResource == success
    currentScene.reset();
    engine.GetLogSystem().AddMessage("Scene stopped successfully.");
    return std::nullopt;
}

farcical::ui::Scene* farcical::game::Game::GetCurrentScene() const {
    return currentScene.get();
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
