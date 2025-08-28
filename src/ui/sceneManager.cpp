//
// Created by dgmuller on 8/17/25.
//
#include "../../include/ui/sceneManager.hpp"
#include "../../include/ui/factory.hpp"
#include "../../include/engine/engine.hpp"
#include "../../include/engine/system/render.hpp"
#include "../../include/engine/system/music.hpp"

farcical::ui::SceneManager::SceneManager(engine::Engine& engine) : LogInterface(engine.GetLogSystem()),
                                                                   engine{engine},
                                                                   resourceManager{engine.GetResourceManager()},
                                                                   currentScene{nullptr} {
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::LoadResourceIndex(std::string_view indexPath) {
    WriteToLog("Loading SceneIndex from " + std::string{indexPath} + "...");
    ResourceID indexID{sceneIndexDocumentID};
    const auto& createIndexHandle{
        resourceManager.CreateResourceHandle(indexID, ResourceHandle::Type::JSONDocument, indexPath)
    };
    if(!createIndexHandle.has_value()) {
        return createIndexHandle.error();
    } // if createIndexHandle == failure

    const auto& loadIndex{
        resourceManager.GetJSONDoc(indexID)
    };
    if(!loadIndex.has_value()) {
        return loadIndex.error();
    } // if loadIndex == failure

    const nlohmann::json& indexJSON{*loadIndex.value()};
    for(const auto& sceneJSON: indexJSON) {
        const auto& findID{sceneJSON.find("id")};
        if(findID == sceneJSON.end()) {
            const std::string failMsg{"Invalid configuration: No ID found for Scene in SceneIndex."};
            return engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg};
        } // if ID not found
        const engine::EntityID sceneID{findID.value().get<std::string>()};

        const auto& findResource{sceneJSON.find("resource")};
        if(findResource == sceneJSON.end()) {
            const std::string failMsg{
                "Invalid configuration: No Resource found for Scene (id=\"" + sceneID + "\") in SceneIndex."
            };
            return engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg};
        } // if resource not found
        const auto& resourceJSON{findResource.value()};

        const auto& findResourceID{resourceJSON.find("id")};
        if(findResourceID == resourceJSON.end()) {
            const std::string failMsg{
                "Invalid configuration: No ResourceID found for Scene (id=\"" + sceneID + "\") in SceneIndex."
            };
            return engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg};
        } // if resourceID not found

        const auto& findResourcePath{resourceJSON.find("path")};
        if(findResourcePath == resourceJSON.end()) {
            const std::string failMsg{
                "Invalid configuration: No path found for Scene (id=\"" + sceneID + "\") in SceneIndex."
            };
            return engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg};
        }
        const ResourceParameters resourceParameters{
            findResourceID.value().get<std::string>(), findResourcePath.value().get<std::string>()
        };
        resourceIndex.insert(std::make_pair(sceneID, resourceParameters));
    } // for each Scene in index

    const auto& buildPropertiesCache{BuildPropertiesCache()};
    if(buildPropertiesCache.has_value()) {
        return buildPropertiesCache.value();
    } // if buildPropertiesCache == failure

    WriteToLog("SceneIndex successfully loaded.");

    return std::nullopt;
}

std::expected<farcical::ui::SceneProperties, farcical::engine::Error>
farcical::ui::SceneManager::GetCurrentSceneProperties() const {
    const auto& findSceneResult{FindSceneProperties(currentScene->GetID())};
    if(!findSceneResult.has_value()) {
        return std::unexpected(findSceneResult.error());
    } // if findSceneResult == failure
    return findSceneResult.value();
}

std::expected<farcical::ui::SceneProperties, farcical::engine::Error> farcical::ui::SceneManager::FindSceneProperties(
    engine::EntityID id) const {
    const auto& findProperties{propertiesCache.find(id)};
    if(findProperties == propertiesCache.end()) {
        const std::string failMsg{"Error: Scene (id=\"" + id + "\") not found."};
        return std::unexpected(engine::Error{engine::Error::Signal::ResourceNotFound, failMsg});
    } // if findProperties == failure
    return findProperties->second;
}

farcical::ui::Scene* farcical::ui::SceneManager::GetCurrentScene() const {
    return currentScene.get();
}

std::expected<farcical::ui::Scene*, farcical::engine::Error> farcical::ui::SceneManager::SetCurrentScene(
    engine::EntityID id) {
    /*
        STEP ZERO: Create the Scene and load its SceneProperties from cache
    */
    WriteToLog("Creating Scene (id=\"" + id + "\")...");
    currentScene = std::make_unique<Scene>(id);
    const auto& getPropertiesResult{GetCurrentSceneProperties()};
    if(!getPropertiesResult.has_value()) {
        return std::unexpected(getPropertiesResult.error());
    } // if getPropertiesResult == failure
    const SceneProperties& properties{getPropertiesResult.value()};

    /*
        STEP ONE: Cache Resources
        (Fonts, Textures, RepeatingTextures, SegmentedTextures, BorderTexture)
    */
    const auto& buildCacheResult{BuildResourceCache(properties)};
    if(buildCacheResult.has_value()) {
        return std::unexpected(buildCacheResult.value());
    } // if buildCacheResult == failure

    /*
        STEP TWO: Create RenderContext in RenderSystem
    */
    engine::RenderSystem& renderSystem{engine.GetRenderSystem()};
    const auto& createRenderContext{
        renderSystem.CreateRenderContext(id)
    };
    if(!createRenderContext.has_value()) {
        return std::unexpected(createRenderContext.error());
    } // if createRenderContext == failure

    /*
        STEP TWO: Create Widgets using the structure specified in Layout
    */
    for(auto layer: properties.layout.layers) {
        for(auto& decorationProperties: layer.decorationProperties) {
            decorationProperties.layerID = layer.id;
            const auto& createDecoration{
                factory::CreateDecoration(renderSystem, currentScene.get(), decorationProperties)
            };
            if(!createDecoration.has_value()) {
                return std::unexpected(createDecoration.error());
            } // if createDecoration == failure
        } // for each Decoration

        layer.titleProperties.layerID = layer.id;
        const auto& createTitle{
            factory::CreateText(renderSystem, currentScene.get(), layer.titleProperties)
        };
        if(!createTitle.has_value()) {
            return std::unexpected(createTitle.error());
        } // if createTitle == failure

        for(auto& heading: layer.headingProperties) {
            heading.layerID = layer.id;
            const auto& createHeading{
                factory::CreateText(renderSystem, currentScene.get(), heading)
            };
            if(!createHeading.has_value()) {
                return std::unexpected(createHeading.error());
            } // if createHeading == failure
        } // for each heading
        layer.menuProperties.layerID = layer.id;
        const auto& createMenu{
            factory::CreateMenu(
                engine.GetRenderSystem(),
                engine.GetInputSystem(),
                engine.GetEventSystem(),
                currentScene.get(),
                layer.menuProperties)
        };
        if(!createMenu.has_value()) {
            return std::unexpected(createMenu.error());
        } // if createMenu == failure
    } // for each Layer

    if(!properties.music.id.empty()) {
        engine::MusicSystem& musicSystem{engine.GetMusicSystem()};
        if(musicSystem.GetCurrentMusic() != properties.music.id) {
            musicSystem.SetCurrentMusic(properties.music.id);
            musicSystem.PlayMusic();
            musicSystem.SetLoop(properties.music.loop);
        } // if currentMusic does not match properties.music
    } // if Music

    WriteToLog("Scene (id=\"" + id + "\") successfully created.");
    return currentScene.get();
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::DestroyCurrentScene() {
    if(!currentScene) {
        return std::nullopt;
    } // if there is no currentScene

    WriteToLog("Destroying Scene (id=\"" + currentScene->GetID() + "\")...");

    const auto& findProperties{GetCurrentSceneProperties()};
    if(!findProperties.has_value()) {
        return findProperties.error();
    } // if SceneProperties not found
    const SceneProperties& properties{findProperties.value()};
    /*
        STEP ZERO: Destroy Widgets using the structure specified in Layout
    */
    engine::RenderSystem& renderSystem{engine.GetRenderSystem()};
    for(const auto& layer: properties.layout.layers) {
        for(const auto& decorationProperties: layer.decorationProperties) {
            Decoration* decoration{dynamic_cast<Decoration*>(currentScene->FindChild(decorationProperties.id))};
            if(decoration) {
                const auto& destroyDecoration{
                    factory::DestroyDecoration(renderSystem, currentScene.get(), decoration)
                };
                if(destroyDecoration.has_value()) {
                    return destroyDecoration.value();
                } // if destroyDecoration == failure
            } // if Decoration found
        } // for each Decoration

        if(!layer.titleProperties.id.empty()) {
            Text* title{dynamic_cast<Text*>(currentScene->FindChild(layer.titleProperties.id))};
            if(title) {
                const auto& destroyTitle{
                    factory::DestroyText(renderSystem, currentScene.get(), title)
                };
                if(destroyTitle.has_value()) {
                    return destroyTitle.value();
                } // if destroyTitle == failure
            } // if Title found
        } // if Title has valid ID

        for(const auto& heading: layer.headingProperties) {
            Text* headingText{dynamic_cast<Text*>(currentScene->FindChild(heading.id))};
            const auto& destroyHeading{
                factory::DestroyText(renderSystem, currentScene.get(), headingText)
            };
            if(destroyHeading.has_value()) {
                return destroyHeading.value();
            } // if destroyHeading == failure
        } // for each heading

        if(!layer.menuProperties.id.empty()) {
            Menu* menu{dynamic_cast<Menu*>(currentScene->FindChild(layer.menuProperties.id))};
            if(menu) {
                const auto& destroyMenu{
                    factory::DestroyMenu(
                        engine.GetRenderSystem(),
                        engine.GetInputSystem(),
                        engine.GetEventSystem(),
                        currentScene.get(),
                        menu)
                };
                if(destroyMenu.has_value()) {
                    return destroyMenu.value();
                } // if destroyMenu == failure
            } // if Menu found
        } // if Menu has valid ID
    } // for each Layer

    /*
        STEP ONE: Destroy RenderContext in RenderSystem
    */
    const auto& destroyRenderContext{
        renderSystem.DestroyRenderContext(properties.id)
    };
    if(destroyRenderContext.has_value()) {
        return destroyRenderContext.value();
    } // if destroyRenderContext == failure

    if(!properties.music.id.empty()) {
        if(!properties.music.persist) {
            engine::MusicSystem& musicSystem{engine.GetMusicSystem()};
            musicSystem.StopMusic();
        } // if persist flag is false
    } // if Music

    /*
        STEP TWO: Destroy Resource Caches
        (Fonts, Textures, RepeatingTextures, SegmentedTextures, BorderTexture)
    */
    const auto& destroyCacheResult{DestroyResourceCache(properties)};
    if(destroyCacheResult.has_value()) {
        return destroyCacheResult.value();
    } // if destroyCacheResult == failure

    currentScene.reset(nullptr);
    WriteToLog("Scene (id=\"" + properties.id + "\") successfully destroyed.");

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::BuildPropertiesCache() {
    for(const auto& sceneResource: resourceIndex) {
        const engine::EntityID& sceneID{sceneResource.first};
        const ResourceID& documentID{sceneResource.second.first};
        const std::string& documentPath{engine.GetConfig().scenePath + "/" + sceneResource.second.second};
        WriteToLog("Loading JSONDocument for Scene (id=\"" + sceneID + "\")...");
        const auto& createDocumentHandle{
            engine.GetResourceManager().CreateResourceHandle(
                documentID,
                ResourceHandle::Type::JSONDocument,
                documentPath)
        };
        if(!createDocumentHandle.has_value()) {
            return createDocumentHandle.error();
        } // if createDocumentHandle == failure

        const ResourceHandle& documentHandle{*createDocumentHandle.value()};
        // Request the JSONDocument from the ResourceManager
        const auto& requestJSONDoc{engine.GetResourceManager().GetJSONDoc(documentHandle.id)};
        if(!requestJSONDoc.has_value()) {
            return requestJSONDoc.error();
        } // if requestJSONDoc == failure

        const auto& sceneJSON{requestJSONDoc.value()};
        const auto& loadSceneResult{LoadScene(*sceneJSON)};
        if(!loadSceneResult.has_value()) {
            return loadSceneResult.error();
        } // if loadSceneResult == failure
        propertiesCache.emplace(sceneID, loadSceneResult.value());
    } // for each Scene in SceneIndex

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::BuildResourceCache(
    const SceneProperties& properties) const {
    /* MUSIC */
    if(!properties.music.id.empty()) {
        const auto& buildMusicCache{
            BuildMusicCache(std::vector{properties.music})
        };
        if(buildMusicCache.has_value()) {
            return buildMusicCache.value();
        } // if buildMusicCache == failure
    } // if music

    /*  FONTS   */
    const auto& buildFontCache{
        BuildFontCache(properties.fonts)
    };
    if(buildFontCache.has_value()) {
        return buildFontCache.value();
    } // if buildFontCache == failure

    /* TEXTURES */
    const auto& buildTextureCache{
        BuildTextureCache(properties.textures)
    };
    if(buildTextureCache.has_value()) {
        return buildTextureCache.value();
    } // if buildTextureCache == failure

    /* REPEATING TEXTURES */
    const auto& buildRepeatingTextureCache{
        BuildRepeatingTextureCache(properties.repeatingTextures)
    };
    if(buildRepeatingTextureCache.has_value()) {
        return buildRepeatingTextureCache.value();
    } // if buildRepeatingTextureCache == failure

    /* SEGMENTED TEXTURES */
    const auto& buildSegmentedTextureCache{
        BuildSegmentedTextureCache(properties.segmentedTextures)
    };
    if(buildSegmentedTextureCache.has_value()) {
        return buildSegmentedTextureCache.value();
    } // if buildSegmentedTextureCache == failure

    /* BORDER TEXTURES */
    const auto& buildBorderTextureCache{
        BuildBorderTextureCache(properties.borderTexture)
    };
    if(buildBorderTextureCache.has_value()) {
        return buildBorderTextureCache.value();
    } // if buildBorderTextureCache == failure

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::DestroyResourceCache(
    const SceneProperties& properties) const {
    /* MUSICS */
    currentScene->ClearMusicCache();
    currentScene->ClearMusicPropertiesCache();
    const auto& destroyMusicCache{DestroyMusicCache(std::vector{properties.music})};
    if(destroyMusicCache.has_value()) {
        return destroyMusicCache.value();
    } // if destroyMusicCache == failure

    /* FONTS */
    currentScene->ClearFontCache();
    currentScene->ClearFontPropertiesCache();
    const auto& destroyFontCache{DestroyFontCache(properties.fonts)};
    if(destroyFontCache.has_value()) {
        return destroyFontCache.value();
    } // if destroyFontCache == failure

    /* TEXTURES */
    currentScene->ClearTextureCache();
    currentScene->ClearTexturePropertiesCache();
    const auto& destroyTextureCache{DestroyTextureCache(properties.textures)};
    if(destroyTextureCache.has_value()) {
        return destroyTextureCache.value();
    } // if destroyTextureCache == failure

    /* REPEATING TEXTURES */
    const auto& destroyRepeatingTextureCache{DestroyRepeatingTextureCache(properties.repeatingTextures)};
    if(destroyRepeatingTextureCache.has_value()) {
        return destroyRepeatingTextureCache.value();
    } // if destroyRepeatingTextureCache == failure

    /* SEGMENTED TEXTURES */
    const auto& destroySegmentedTextureCache{DestroySegmentedTextureCache(properties.segmentedTextures)};
    if(destroySegmentedTextureCache.has_value()) {
        return destroySegmentedTextureCache.value();
    } // if destroySegmentedTextureCache == failure

    /* BORDER TEXTURES */
    const auto& destroyBorderTextureCache{DestroyBorderTextureCache(properties.borderTexture)};
    if(destroyBorderTextureCache.has_value()) {
        return destroyBorderTextureCache.value();
    } // if destroyBorderTextureCache == failure

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::BuildMusicCache(
    const std::vector<MusicProperties>& musics) const {
    for(const auto& musicProperties: musics) {
        ResourceHandle* handle{resourceManager.GetResourceHandle(musicProperties.id)};
        if(handle) {
            if(!musicProperties.persist) {
                const std::string failMsg{
                    "Error: Attempted to create Resource (id=\"" + musicProperties.id + "\"), but it already exists!"
                };
                return engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg};
            } // if persist flag is not set
        } // if this ResourceHandle already exists
        else {
            const auto& createHandle{
                resourceManager.CreateResourceHandle(musicProperties.id, ResourceHandle::Type::Music,
                                                     musicProperties.path)
            };
            if(!createHandle.has_value()) {
                return createHandle.error();
            } // if createHandle == failure
            handle = createHandle.value();
        } // else if ResourceHandle does not already exist
        const auto& loadMusic{resourceManager.GetMusic(musicProperties.id)};
        if(!loadMusic.has_value()) {
            return loadMusic.error();
        } // if loadMusic == failure
        currentScene->CacheMusic(musicProperties.id, loadMusic.value());
        currentScene->CacheMusicProperties(musicProperties.id, musicProperties);
    } // for each Music
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::DestroyMusicCache(
    const std::vector<MusicProperties>& musics) const {
    for(const auto& musicProperties: musics) {
        if(musicProperties.persist) {
            continue;
        } // skip any Music with (persist flag == true)
        const auto& destroyHandle{
            resourceManager.DestroyResourceHandle(musicProperties.id, ResourceHandle::Type::Music)
        };
        if(destroyHandle.has_value()) {
            return destroyHandle.value();
        } // if destroyHandle == failure
    } // for each Music
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::BuildFontCache(
    const std::vector<FontProperties>& fonts) const {
    for(const auto& fontProperties: fonts) {
        const auto& createHandle{
            resourceManager.CreateResourceHandle(fontProperties.id, ResourceHandle::Type::Font, fontProperties.path)
        };
        if(!createHandle.has_value()) {
            return createHandle.error();
        } // if createHandle == failure
        const auto& loadFont{resourceManager.GetFont(fontProperties.id)};
        if(!loadFont.has_value()) {
            return loadFont.error();
        } // if loadFont == failure
        currentScene->CacheFont(fontProperties.id, loadFont.value());
        currentScene->CacheFontProperties(fontProperties.id, fontProperties);
    } // for each Font
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::DestroyFontCache(
    const std::vector<FontProperties>& fonts) const {
    for(const auto& fontProperties: fonts) {
        if(fontProperties.persist) {
            continue;
        } // skip any Font with (persist flag == true)
        const auto& destroyHandle{
            resourceManager.DestroyResourceHandle(fontProperties.id, ResourceHandle::Type::Font)
        };
        if(destroyHandle.has_value()) {
            return destroyHandle.value();
        } // if destroyHandle == failure
    } // for each Font
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::BuildTextureCache(
    const std::vector<TextureProperties>& textures) const {
    for(const auto& textureProperties: textures) {
        const auto& createHandle{
            resourceManager.CreateResourceHandle(textureProperties.id, ResourceHandle::Type::Texture,
                                                 textureProperties.path)
        };
        if(!createHandle.has_value()) {
            return createHandle.error();
        } // if createHandle == failure
        const auto& loadTexture{resourceManager.GetTexture(textureProperties)};
        if(!loadTexture.has_value()) {
            return loadTexture.error();
        } // if loadTexture == failure
        currentScene->CacheTexture(textureProperties.id, loadTexture.value());
        currentScene->CacheTextureProperties(textureProperties.id, textureProperties);
    } // for each Texture
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::DestroyTextureCache(
    const std::vector<TextureProperties>& textures) const {
    for(const auto& textureProperties: textures) {
        if(textureProperties.persist) {
            continue;
        } // skip any Texture with (persist flag == true)
        const auto& destroyHandle{
            resourceManager.DestroyResourceHandle(textureProperties.id, ResourceHandle::Type::Texture)
        };
        if(destroyHandle.has_value()) {
            return destroyHandle.value();
        } // if destroyHandle == failure
    } // for each Texture
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::BuildRepeatingTextureCache(
    const std::vector<RepeatingTextureProperties>& textures) const {
    for(const auto& textureProperties: textures) {
        const auto& createInputHandle{
            resourceManager.CreateResourceHandle(textureProperties.inputID, ResourceHandle::Type::Texture,
                                                 textureProperties.path)
        };
        if(!createInputHandle.has_value()) {
            return createInputHandle.error();
        } // if createInputHandle == failure

        const auto& createOutputHandle{
            resourceManager.CreateResourceHandle(textureProperties.outputID, ResourceHandle::Type::Texture,
                                                 textureProperties.path)
        };
        if(!createOutputHandle.has_value()) {
            return createOutputHandle.error();
        } // if createOutputHandle == failure

        const auto& loadTexture(resourceManager.GetTexture(textureProperties));
        if(!loadTexture.has_value()) {
            return loadTexture.error();
        } // if loadTexture == failure
        currentScene->CacheTexture(textureProperties.outputID, loadTexture.value());
        currentScene->CacheTextureProperties(
            textureProperties.outputID, TextureProperties{
                textureProperties.outputID,
                textureProperties.path,
                textureProperties.scale,
                textureProperties.inputRect
            });
    } // for each RepeatingTexture
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::DestroyRepeatingTextureCache(
    const std::vector<RepeatingTextureProperties>& textures) const {
    for(const auto& textureProperties: textures) {
        if(textureProperties.persist) {
            continue;
        } // skip any Texture with (persist flag == true)
        const auto& destroyInputHandle{
            resourceManager.DestroyResourceHandle(textureProperties.inputID, ResourceHandle::Type::Texture)
        };
        if(destroyInputHandle.has_value()) {
            return destroyInputHandle.value();
        } // if destroyInputHandle == failure
        const auto& destroyOutputHandle{
            resourceManager.DestroyResourceHandle(textureProperties.outputID, ResourceHandle::Type::Texture)
        };
        if(destroyOutputHandle.has_value()) {
            return destroyOutputHandle.value();
        } // if destroyOutputHandle == failure
    } // for each RepeatingTexture
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::BuildSegmentedTextureCache(
    const std::vector<SegmentedTextureProperties>& textures) const {
    for(const auto& textureProperties: textures) {
        const ResourceID textureID{textureProperties.id + "Texture"};
        const auto& createHandle{
            resourceManager.CreateResourceHandle(
                textureID, ResourceHandle::Type::Texture, textureProperties.path)
        };
        if(!createHandle.has_value()) {
            return createHandle.error();
        } // if createHandle == failure

        std::vector<ResourceID> segmentIDList;
        for(auto segmentProperties: textureProperties.segments) {
            ResourceID segmentID{textureProperties.id};
            segmentID += static_cast<char>(std::toupper(segmentProperties.id[0]));
            segmentID.append(segmentProperties.id.substr(1, segmentProperties.id.length() - 1));
            segmentID.append("Texture");
            segmentProperties.id = segmentID;
            const auto& createSegmentHandle{
                resourceManager.CreateResourceHandle(
                    segmentProperties.id, ResourceHandle::Type::Texture, segmentProperties.path)
            };
            if(!createSegmentHandle.has_value()) {
                return createSegmentHandle.error();
            } // if createSegmentHandle == failure

            const auto& loadTexture{resourceManager.GetTexture(segmentProperties)};
            if(!loadTexture.has_value()) {
                return loadTexture.error();
            } // if loadTexture == failure
            segmentIDList.emplace_back(segmentProperties.id);
        } // for each segment of SegmentedTexture

        const ResourceID splicedTextureID{textureProperties.id + "Texture"};
        const auto& createTexture{
            resourceManager.CreateSplicedTexture(splicedTextureID, segmentIDList)
        };
        if(!createTexture.has_value()) {
            return createTexture.error();
        } // if createTexture == failure

        currentScene->CacheTexture(splicedTextureID, createTexture.value());
        currentScene->CacheTextureProperties(
            splicedTextureID, TextureProperties{
                splicedTextureID,
                textureProperties.path,
                textureProperties.scale,
                sf::IntRect{{0, 0}, {0, 0}}
            });
    } // for each SegmentedTexture
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::DestroySegmentedTextureCache(
    const std::vector<SegmentedTextureProperties>& textures) const {
    for(const auto& textureProperties: textures) {
        if(textureProperties.persist) {
            continue;
        } // skip any Texture with (persist flag == true)
        for(auto segmentProperties: textureProperties.segments) {
            ResourceID segmentID{textureProperties.id};
            segmentID += static_cast<char>(std::toupper(segmentProperties.id[0]));
            segmentID.append(segmentProperties.id.substr(1, segmentProperties.id.length() - 1));
            segmentID.append("Texture");
            segmentProperties.id = segmentID;
            const auto& destroyHandle{
                resourceManager.DestroyResourceHandle(segmentID, ResourceHandle::Type::Texture)
            };
            if(destroyHandle.has_value()) {
                return destroyHandle.value();
            } // if destroyHandle == failure
        } // for each segment of SegmentedTexture
        const ResourceID textureID{textureProperties.id + "Texture"};
        const auto& destroyHandle{
            resourceManager.DestroyResourceHandle(textureID, ResourceHandle::Type::Texture)
        };
        if(destroyHandle.has_value()) {
            return destroyHandle.value();
        } // if destroyHandle == failure
    } // for each SegmentedTexture
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::BuildBorderTextureCache(
    const BorderTextureProperties& properties) const {
    if(properties.id.empty()) {
        return std::nullopt;
    } // if no borderTexture specified

    const auto& createHandle{
        resourceManager.CreateResourceHandle(properties.id, ResourceHandle::Type::Texture, properties.path)
    };
    if(!createHandle.has_value()) {
        return createHandle.error();
    } // if createHandle == failure

    std::vector<ResourceID> cornerTextureIDs;
    for(const auto& corner: properties.cornerTextures) {
        const auto& createCornerHandle{
            resourceManager.CreateResourceHandle(corner.id, ResourceHandle::Type::Texture, corner.path)
        };
        if(!createCornerHandle.has_value()) {
            return createCornerHandle.error();
        } // if createCornerHandle == failure
        const auto& loadCornerTexture{
            resourceManager.GetTexture(corner)
        };
        if(!loadCornerTexture.has_value()) {
            return loadCornerTexture.error();
        } // if loadCornerTexture == failure
        currentScene->CacheTexture(corner.id, loadCornerTexture.value());
        currentScene->CacheTextureProperties(corner.id, corner);
        cornerTextureIDs.emplace_back(corner.id);
    } // for each corner

    std::vector<ResourceID> edgeTextureIDs;
    for(const auto& edge: properties.edgeTextures) {
        const auto& createEdgeHandle{
            resourceManager.CreateResourceHandle(edge.id, ResourceHandle::Type::Texture, edge.path)
        };
        if(!createEdgeHandle.has_value()) {
            return createEdgeHandle.error();
        } // if createEdgeHandle == failure
        const auto& loadEdgeTexture{
            resourceManager.GetTexture(edge)
        };
        if(!loadEdgeTexture.has_value()) {
            return loadEdgeTexture.error();
        } // if loadEdgeTexture == failure
        currentScene->CacheTexture(edge.id, loadEdgeTexture.value());
        currentScene->CacheTextureProperties(edge.id, edge);
        edgeTextureIDs.emplace_back(edge.id);
    } // for each edge

    const auto& createCenterHandle{
        resourceManager.CreateResourceHandle(
            properties.centerTexture.id,
            ResourceHandle::Type::Texture,
            properties.centerTexture.path)
    };
    if(!createCenterHandle.has_value()) {
        return createCenterHandle.error();
    } // if createCenterHandle == failure
    const auto& loadCenterTexture{
        resourceManager.GetTexture(properties.centerTexture)
    };
    if(!loadCenterTexture.has_value()) {
        return loadCenterTexture.error();
    } // if loadCenterTexture == failure
    currentScene->CacheTexture(properties.centerTexture.id, loadCenterTexture.value());
    currentScene->CacheTextureProperties(properties.centerTexture.id, properties.centerTexture);

    const auto& windowSize{engine.GetRenderSystem().GetWindow().getSize()};
    const sf::Vector2u outputSize{
        static_cast<unsigned int>(
            static_cast<float>(properties.percentSize.x) * static_cast<float>(windowSize.x) / 100.0 / properties.scale),
        static_cast<unsigned int>(
            static_cast<float>(properties.percentSize.y) * static_cast<float>(windowSize.y) / 100.0 / properties.scale)
    };
    const auto& createBorderTexture{
        resourceManager.CreateBorderTexture(
            properties.id,
            outputSize,
            cornerTextureIDs,
            edgeTextureIDs,
            properties.centerTexture.id)
    };
    if(!createBorderTexture.has_value()) {
        return createBorderTexture.error();
    } // if createBorderTexture == failure

    currentScene->CacheTexture(properties.id, createBorderTexture.value());
    currentScene->CacheTextureProperties(
        properties.id, TextureProperties{
            properties.id,
            properties.path,
            properties.scale,
            sf::IntRect{{0, 0}, {0, 0}}
        });

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::ui::SceneManager::DestroyBorderTextureCache(
    const BorderTextureProperties& properties) const {
    if(properties.persist) {
        return std::nullopt;
    } // skip if (persist flag == true)
    for(const auto& corner: properties.cornerTextures) {
        const auto& destroyCornerHandle{
            resourceManager.DestroyResourceHandle(corner.id, ResourceHandle::Type::Texture)
        };
        if(destroyCornerHandle.has_value()) {
            return destroyCornerHandle.value();
        } // if destroyCornerHandle == failure
    } // for each corner

    for(const auto& edge: properties.edgeTextures) {
        const auto& destroyEdgeHandle{
            resourceManager.DestroyResourceHandle(edge.id, ResourceHandle::Type::Texture)
        };
        if(destroyEdgeHandle.has_value()) {
            return destroyEdgeHandle.value();
        } // if destroyEdgeHandle == failure
    } // for each edge

    const auto& destroyCenterHandle{
        resourceManager.DestroyResourceHandle(properties.centerTexture.id, ResourceHandle::Type::Texture)
    };
    if(destroyCenterHandle.has_value()) {
        return destroyCenterHandle.value();
    } // if destroyCenterHandle == failure

    const auto& destroyHandle{
        resourceManager.DestroyResourceHandle(properties.id, ResourceHandle::Type::Texture)
    };
    if(destroyHandle.has_value()) {
        return destroyHandle.value();
    } // if destroyHandle == failure

    return std::nullopt;
}
