//
// Created by dgmuller on 6/24/25.
//
#include "../../include/ui/scene.hpp"

farcical::ui::Scene::Scene(engine::EntityID id, game::Game& game): id{id},
                                                                   rootContainer{
                                                                       std::make_unique<RootContainer>(
                                                                           std::string{rootContainerID}, game)
                                                                   } {
}

farcical::engine::EntityID farcical::ui::Scene::GetID() const {
    return id;
}

farcical::ui::RootContainer& farcical::ui::Scene::GetRootContainer() const {
    return *rootContainer;
}

std::vector<farcical::ui::Widget*> farcical::ui::Scene::GetTopLevelWidgets() const {
    return rootContainer->GetChildren();
}

farcical::ui::Widget* farcical::ui::Scene::FindWidget(engine::EntityID widgetID) const {
    if(rootContainer->GetID() == widgetID) {
        return rootContainer.get();
    } // if rootContainer matches widgetID
    return rootContainer->FindChild(widgetID);
}

void farcical::ui::Scene::AddWidget(std::unique_ptr<ui::Widget> widget, ui::Widget* parent) {
    // If parent == nullptr, set parent = rootContainer
    if(!parent) {
        parent = rootContainer.get();
    } // if !parent
    if(!parent->IsContainer()) {
        return; // Error!
    } // !parent->IsContainer()
    Container* parentContainer{dynamic_cast<Container*>(parent)};
    if(!parentContainer) {
        return; // Error!
    } // !parentContainer
    parentContainer->AddChild(std::move(widget));
}

void farcical::ui::Scene::RemoveWidget(engine::EntityID widgetID, ui::Widget* parent) {
    // If parent == nullptr, set parent = rootContainer
    if(!parent) {
        parent = rootContainer.get();
    } // if !parent
    if(!parent->IsContainer()) {
        return; // Error!
    } // !parent->IsContainer()
    Container* parentContainer{dynamic_cast<Container*>(parent)};
    if(!parentContainer) {
        return; // Error!
    } // !parentContainer
    if(parentContainer->GetID() == widgetID) {
        parent = parentContainer->GetParent();
        parentContainer = dynamic_cast<Container*>(parent);
        if(parentContainer) {
            parentContainer->RemoveChild(widgetID);
        } // remove from parent
    } // if parentContainer matches widgetID, remove it from its parent
    else {
        Widget* child{parentContainer->FindChild(widgetID)};
        if(child) {
            parentContainer->RemoveChild(widgetID);
            child = nullptr;
        } // if child found
    } // else if parentContainer does not match widgetID
}

sf::Font* farcical::ui::Scene::GetCachedFont(ResourceID id) const {
    const auto& findFont{fontCache.find(id)};
    if(findFont != fontCache.end()) {
        return findFont->second;
    }
    return nullptr;
}

farcical::FontProperties farcical::ui::Scene::GetCachedFontProperties(ResourceID id) const {
    const auto& findProperties{fontPropertiesCache.find(id)};
    if(findProperties != fontPropertiesCache.end()) {
        return findProperties->second;
    }
    return FontProperties{};
}

sf::Texture* farcical::ui::Scene::GetCachedTexture(ResourceID id) const {
    const auto& findTexture{textureCache.find(id)};
    if(findTexture != textureCache.end()) {
        return findTexture->second;
    }
    return nullptr;
}

farcical::TextureProperties farcical::ui::Scene::GetCachedTextureProperties(ResourceID id) const {
    const auto& findProperties{texturePropertiesCache.find(id)};
    if(findProperties != texturePropertiesCache.end()) {
        return findProperties->second;
    }
    return TextureProperties{};
}

void farcical::ui::Scene::CacheFont(ResourceID id, sf::Font* font) {
    fontCache.insert(std::make_pair(id, font));
}

void farcical::ui::Scene::CacheFontProperties(ResourceID id, const FontProperties& fontProperties) {
    fontPropertiesCache.insert(std::make_pair(id, fontProperties));
}

void farcical::ui::Scene::CacheTexture(ResourceID id, sf::Texture* texture) {
    textureCache.insert(std::make_pair(id, texture));
}

void farcical::ui::Scene::CacheTextureProperties(ResourceID id, const TextureProperties& textureProperties) {
    texturePropertiesCache.insert(std::make_pair(id, textureProperties));
}
