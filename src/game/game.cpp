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

farcical::game::GameController::GameController(Game& game): EventHandler(),
                                                            LogInterface(game.GetEngine().GetLogSystem()),
                                                            game{game} {
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
    return;
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
    std::unique_ptr<ui::Scene> scene{std::make_unique<ui::Scene>(config.id)};
    const auto& findConfig{sceneConfigs.find(config.id)};
    if(findConfig != sceneConfigs.end()) {
        sceneConfigs.erase(findConfig);
    } // if a config for this Scene already exists, erase it
    const auto& createSceneResult{sceneConfigs.emplace(std::make_pair(config.id, config))};
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
        CreateDecorations(layerConfig.decorations);
        CreateTitle(layerConfig.title);
        CreateMenu(layerConfig.menu);
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
        const auto& destroyDecorationResult{DestroyDecorations(layerConfig.decorations)};
        if(destroyDecorationResult.has_value()) {
            return destroyDecorationResult.value();
        } // if destroyDecorationResult == failure

        const auto& destroyTitleResult{DestroyTitle(layerConfig.title)};
        if(destroyTitleResult.has_value()) {
            return destroyTitleResult.value();
        } // if destroyTitleResult == failure

        const auto& destroyMenuResult{DestroyMenu(layerConfig.menu)};
        if(destroyMenuResult.has_value()) {
            return destroyMenuResult.value();
        } // if destroyMenuResult == failure
    } // for each layer in layoutConfig

    // Destroy RenderContext
    engine.GetRenderSystem().DestroyRenderContext(currentScene->GetID());

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateDecorations(const std::vector<ui::DecorationConfig>& decorationConfigList) {
    engine::LogSystem& logSystem{engine.GetLogSystem()};
    const auto& windowSize{engine.GetWindow().getSize()};
    for(const auto& decorationConfig: decorationConfigList) {
        // Test length of decorationID to determine if this config is valid
        if(!decorationConfig.id.empty()) {
            // Load Texture from Scene cache
            sf::Texture* texture{currentScene->GetCachedTexture(decorationConfig.textureProperties.id)};
            if(!texture) {
                const std::string failMsg{"NullPtr Error: Failed to retrieve Texture from Scene cache."};
                return engine::Error{engine::Error::Signal::NullPtr, failMsg};
            } // if texture == nullptr

            // Create the Decoration
            ui::Widget* parent{currentScene->FindWidget(decorationConfig.parentID)};
            if(!parent) {
                parent = &currentScene->GetWidgetContainer();
            } // if !parent
            const auto& createDecoration{ui::Decoration::Create(decorationConfig.id, texture, parent)};
            if(createDecoration.has_value()) {
                ui::Decoration* decoration{createDecoration.value()};
                logSystem.AddMessage("Decoration created (id=" + decoration->GetID() + ").");
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

std::optional<farcical::engine::Error> farcical::game::Game::DestroyDecorations(
    const std::vector<ui::DecorationConfig>& decorationConfigList) {
    engine::LogSystem& logSystem{engine.GetLogSystem()};

    for(const auto& decorationConfig: decorationConfigList) {
        // Test length of decorationID to determine if this config is valid
        if(!decorationConfig.id.empty()) {
            engine.GetRenderSystem().DestroyRenderComponent(currentScene->GetID(), decorationConfig.id);
            ui::Widget* parent{currentScene->FindWidget(decorationConfig.parentID)};
            if(parent && parent->IsContainer()) {
                ui::Container* container{dynamic_cast<ui::Container*>(parent)};
                container->RemoveChild(decorationConfig.id);
                logSystem.AddMessage("Decoration destroyed (id=" + decorationConfig.id + ").");
            } // if parent exists and isContainer
        } // if decorationConfig has valid ID
    } // for each decorationConfig in layerConfig.decorations

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateTitle(const ui::LabelConfig& titleConfig) {
    farcical::engine::LogSystem& logSystem{engine.GetLogSystem()};
    const auto& windowSize{engine.GetWindow().getSize()};
    // Test length of labelID to determine if this config is valid
    if(!titleConfig.id.empty()) {
        // Load Font & FontProperties from Scene cache
        sf::Font* font{currentScene->GetCachedFont(titleConfig.fontProperties.id)};
        const FontProperties& fontProperties{currentScene->GetCachedFontProperties(titleConfig.fontProperties.id)};
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
        ui::Widget* parent{currentScene->FindWidget(titleConfig.parentID)};
        if(!parent) {
            parent = &currentScene->GetWidgetContainer();
        } // if !parent
        const auto& createTitle{ui::Label::Create(titleConfig.id, font, fontProperties, parent)};
        if(createTitle.has_value()) {
            logSystem.AddMessage("Title created (id=" + titleConfig.id + ").");
            ui::Label* label{createTitle.value()};
            label->SetContents(titleConfig.contents);
            label->SetScale(sf::Vector2f{label->GetFontProperties().scale, label->GetFontProperties().scale});
            // Create a temporary Text to determine the Label's size
            const sf::Text tempText{*font, titleConfig.contents, fontProperties.characterSize};
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

std::optional<farcical::engine::Error> farcical::game::Game::DestroyTitle(const ui::LabelConfig& titleConfig) {
    engine::LogSystem& logSystem{engine.GetLogSystem()};

    // Test length of labelID to determine if this config is valid
    if(!titleConfig.id.empty()) {
        engine.GetRenderSystem().DestroyRenderComponent(currentScene->GetID(), titleConfig.id);
        ui::Widget* parent{currentScene->FindWidget(titleConfig.parentID)};
        if(parent && parent->IsContainer()) {
            ui::Container* container{dynamic_cast<ui::Container*>(parent)};
            container->RemoveChild(titleConfig.id);
            logSystem.AddMessage("Title destroyed (id=" + titleConfig.id + ").");
        } // if parent exists and isContainer
    } // if title has valid ID
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::CreateMenu(const ui::MenuConfig& menuConfig) {
    engine::LogSystem& logSystem{engine.GetLogSystem()};
    const auto& windowSize{engine.GetWindow().getSize()};
    // Test length of menuID to determine if this config is valid
    if(!menuConfig.id.empty()) {
        // Load Font & FontProperties from Scene cache
        sf::Font* font{currentScene->GetCachedFont(menuConfig.fontProperties.id)};
        const auto& fontProperties{currentScene->GetCachedFontProperties(menuConfig.fontProperties.id)};

        // Create a list of all Button Textures
        std::vector<sf::Texture*> buttonTextures;
        sf::Vector2u textureSize{0, 0};
        for(const auto& textureProperties: menuConfig.buttonConfig.textureProperties | std::views::values) {
            const ResourceID textureID{textureProperties.id + "Texture"};
            sf::Texture* texture{currentScene->GetCachedTexture(textureID)};
            if(texture) {
                buttonTextures.push_back(texture);
                if(texture->getSize().x > textureSize.x && texture->getSize().y > textureSize.y) {
                    textureSize = texture->getSize();
                }
            } // if texture
        } // for each texture in buttonConfig

        // Create 3 lists: names, contents, and onSelection Events for all MenuItems
        std::vector<std::string> idList;
        std::vector<std::string> contentsList;
        std::vector<engine::Event::Type> eventTypeList;
        std::vector<std::vector<std::any>> eventArgsList;
        for(const auto& itemConfig: menuConfig.menuItemConfigs) {
            idList.push_back(itemConfig.id);
            contentsList.push_back(itemConfig.labelConfig.contents);
            eventTypeList.push_back(itemConfig.activationEventType);
            eventArgsList.push_back(itemConfig.activationEventArgs);
        } // for each itemConfig in layerConfig.menu.menuItemConfigs

        ui::MenuItemCollection menuItems{
            idList,
            contentsList,
            eventTypeList,
            eventArgsList
        };

        ui::MenuItemLayout menuLayout{
            ui::MenuItemLayout::Orientation::Vertical,
            textureSize.x / 2,
            {}
        };

        // Create the Menu using the above lists
        ui::Widget* parent{currentScene->FindWidget(menuConfig.parentID)};
        if(!parent) {
            parent = &currentScene->GetWidgetContainer();
        } // if !parent
        const auto& createMenu{
            ui::Menu::Create(
                menuConfig.id,
                parent,
                font,
                fontProperties,
                buttonTextures,
                menuItems,
                menuLayout)
        };
        if(createMenu.has_value()) {
            ui::Menu* menu{createMenu.value()};
            logSystem.AddMessage("Menu created (id=" + menu->GetID() + ").");
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
                sf::Texture* texture{currentScene->GetCachedTexture(textureID)};
                const TextureProperties& textureProperties{currentScene->GetCachedTextureProperties(textureID)};
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
            menuController = std::make_unique<ui::MenuController>(menu, engine.GetEventSystem());
            logSystem.AddMessage("MenuController created.");
            engine.GetInputSystem().AddKeyListener(menuController.get());
            engine.GetInputSystem().AddMouseListener(menuController.get());
        } // if createMenu == success
    } // if Menu has valid ID
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::DestroyMenu(const ui::MenuConfig& menuConfig) {
    engine::LogSystem& logSystem{engine.GetLogSystem()};
    engine::RenderContext* context{engine.GetRenderSystem().GetRenderContext(currentScene->GetID())};

    // Test length of labelID to determine if this config is valid
    if(!menuConfig.id.empty()) {
        // Destroy MenuController
        engine.GetInputSystem().RemoveKeyListener(menuController.get());
        engine.GetInputSystem().RemoveMouseListener(menuController.get());
        menuController.reset();
        logSystem.AddMessage("MenuController destroyed.");

        // Loop through each MenuItem and destroy all its RenderComponents
        for(auto& itemConfig: menuConfig.menuItemConfigs) {
            engine.GetRenderSystem().DestroyRenderComponent(currentScene->GetID(), itemConfig.buttonConfig.id);
            engine.GetRenderSystem().DestroyRenderComponent(currentScene->GetID(), itemConfig.labelConfig.id);
        } // for each itemConfig in menuItemConfigs

        // Destroy Menu
        ui::Widget* parent{currentScene->FindWidget(menuConfig.parentID)};
        if(!parent) {
            parent = &currentScene->GetWidgetContainer();
        } // if !parent
        if(parent->IsContainer()) {
            ui::Container* container{dynamic_cast<ui::Container*>(parent)};
            container->RemoveChild(menuConfig.id);
            logSystem.AddMessage("Menu destroyed (id=" + menuConfig.id + ").");
        } // if parent->IsContainer()
    } // if Menu has valid ID
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
