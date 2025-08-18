//
// Created by dgmuller on 6/24/25.
//
#include "../../include/ui/scene.hpp"

void farcical::ui::Scene::DoAction(Action action) {
}

farcical::ui::Scene::Scene(engine::EntityID id) : Container(id, Widget::Type::Scene, nullptr) {
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

void farcical::ui::Scene::ClearFontCache() {
    fontCache.clear();
}

void farcical::ui::Scene::ClearFontPropertiesCache() {
    fontPropertiesCache.clear();
}

void farcical::ui::Scene::ClearTextureCache() {
    textureCache.clear();
}

void farcical::ui::Scene::ClearTexturePropertiesCache() {
    texturePropertiesCache.clear();
}

std::expected<farcical::ui::MenuController*, farcical::engine::Error> farcical::ui::Scene::CreateMenuController(
    Menu* menu, engine::EventSystem& eventSystem) {
    const auto& createMenuController{
        menuControllers.insert(std::make_pair(menu->GetID(), std::make_unique<MenuController>(menu, eventSystem)))
    };
    if(!createMenuController.second) {
        const std::string failMsg{"Error: Failed to create MenuController for Menu with id=\"" + menu->GetID() + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if createMenuController == failure
    return createMenuController.first->second.get();
}

std::optional<farcical::engine::Error> farcical::ui::Scene::DestroyMenuController(
    Menu* menu, engine::EventSystem& eventSystem) {
    const auto& findMenuController{menuControllers.find(menu->GetID())};
    if(findMenuController == menuControllers.end()) {
        const std::string failMsg{"Error: MenuController for Menu with id=\"" + menu->GetID() + "\" not found."};
        return engine::Error{engine::Error::Signal::ResourceNotFound, failMsg};
    } // if findMenuController == failure
    menuControllers.erase(findMenuController);
    return std::nullopt;
}

farcical::ui::MenuController* farcical::ui::Scene::GetMenuController(engine::EntityID menuID) {
    MenuController* controller{nullptr};
    const auto& findController{menuControllers.find(menuID)};
    if(findController != menuControllers.end()) {
        controller = findController->second.get();
    } // if controller found
    return controller;
}
