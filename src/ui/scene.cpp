//
// Created by dgmuller on 6/24/25.
//
#include "../../include/ui/scene.hpp"
#include "../../include/ui/decoration.hpp"

farcical::ui::Scene::Scene(engine::EntityID id):
    Container(id, Widget::Type::Scene, nullptr) {
}

void farcical::ui::Scene::DoAction(Action action) {
}

std::vector<sf::Text*> farcical::ui::Scene::GetTextLayer(Layout::Layer::ID layerID) const {
    std::vector<sf::Text*> textList;
    const auto& layer{textLayers[static_cast<int>(layerID)]};
    for(const auto& text: layer | std::views::values) {
        textList.emplace_back(const_cast<sf::Text*>(&text));
    }
    return textList;
}

std::vector<sf::Sprite*> farcical::ui::Scene::GetSpriteLayer(Layout::Layer::ID layerID) const {
    std::vector<sf::Sprite*> spriteList;
    const auto& layer{spriteLayers[static_cast<int>(layerID)]};
    for(const auto& sprite: layer | std::views::values) {
        spriteList.emplace_back(const_cast<sf::Sprite*>(&sprite));
    }
    return spriteList;
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
