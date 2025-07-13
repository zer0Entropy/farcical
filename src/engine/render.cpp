//
// Created by dgmuller on 5/24/25.
//

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../../include/engine/render.hpp"

farcical::engine::RenderSystem::RenderSystem(sf::RenderWindow& window): System{ID::RenderSystem},
                                                                        window{window},
                                                                        layers{
                                                                          {.id = ui::Layout::Layer::ID::Background},
                                                                          {.id = ui::Layout::Layer::ID::Foreground},
                                                                          {.id = ui::Layout::Layer::ID::Overlay}
                                                                        } {
}

void farcical::engine::RenderSystem::Init() {
}

void farcical::engine::RenderSystem::Update() {
  if(window.isOpen()) {
    window.clear();
    for(int layerIndex = static_cast<int>(ui::Layout::Layer::ID::Background);
        layerIndex < static_cast<int>(ui::Layout::Layer::ID::NumLayers); ++layerIndex) {
      RenderLayer& layer{layers[layerIndex]};
      for(const auto& component: layer.componentList) {
        if(component->texture) {
          sf::Sprite sprite{*component->texture};
          sprite.setScale(component->scale);
          sprite.setPosition(component->position);
          window.draw(sprite);
        } // if texture
        else if(component->font) {
          const FontProperties& fontProperties{component->fontProperties};
          sf::Text text{*component->font, component->contents, fontProperties.characterSize};
          text.setScale(component->scale);
          text.setPosition(component->position);
          text.setFillColor(fontProperties.color);
          text.setOutlineColor(fontProperties.outlineColor);
          text.setOutlineThickness(fontProperties.outlineThickness);
          window.draw(text);
        } // else if font
      }
    }
    window.display();
  }
}

void farcical::engine::RenderSystem::Stop() {
  if(window.isOpen()) {
    window.close();
  }
}

std::expected<farcical::engine::RenderComponent*, farcical::engine::Error>
farcical::engine::RenderSystem::CreateRenderComponent(
  ui::Layout::Layer::ID layerID, EntityID parentID, sf::Texture* texture) {
  const auto& createComponent{
    components.insert(std::make_pair(parentID, std::make_unique<RenderComponent>(parentID)))
  };
  if(createComponent.second) {
    RenderComponent* component{createComponent.first->second.get()};
    component->texture = texture;
    layers[static_cast<int>(layerID)].componentList.push_back(component);
    return component;
  } // if createComponent == success
  const std::string failMsg{"Invalid configuration: Failed to create RenderComponent for " + parentID + "."};
  return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
}

std::expected<farcical::engine::RenderComponent*, farcical::engine::Error>
farcical::engine::RenderSystem::CreateRenderComponent(
  ui::Layout::Layer::ID layerID, EntityID parentID, sf::Font* font, const FontProperties& fontProperties,
  std::string_view contents) {
  const auto& createComponent{
    components.insert(std::make_pair(parentID, std::make_unique<RenderComponent>(parentID)))
  };
  if(createComponent.second) {
    RenderComponent* component{createComponent.first->second.get()};
    component->font = font;
    component->fontProperties = fontProperties;
    component->contents = contents;
    layers[static_cast<int>(layerID)].componentList.push_back(component);
    return component;
  } // if createComponent == success
  const std::string failMsg{"Invalid configuration: Failed to create RenderComponent for " + parentID + "."};
  return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
}

std::optional<farcical::engine::Error> farcical::engine::RenderSystem::DestroyRenderComponent(EntityID parentID) {
  const auto& findComponent{components.find(parentID)};
  if(findComponent != components.end()) {
    for(int index = 0; index < static_cast<int>(ui::Layout::Layer::ID::NumLayers); ++index) {
      bool found{false};
      for(auto componentIter = layers[index].componentList.begin();
          !found && componentIter != layers[index].componentList.end(); ++componentIter) {
        if((*componentIter)->parentID == parentID) {
          layers[index].componentList.erase(componentIter);
          found = true;
        } // if Component found
      } // for each Component in componentList
      if(found) {
        break;
      }
    } // for each layer

    components.erase(findComponent);
  } // if Component found
  return std::nullopt;
}
