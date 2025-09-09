//
// Created by dgmuller on 5/24/25.
//

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../../../include/engine/system/render.hpp"

farcical::engine::RenderSystem::RenderSystem(sf::RenderWindow& window, LogSystem& logSystem, ErrorGenerator* errorGenerator):
  System(ID::RenderSystem, logSystem, errorGenerator),
  window{window} {
}

void farcical::engine::RenderSystem::Init() {
  WriteToLog("RenderSystem initialized.");
}

void farcical::engine::RenderSystem::Update() {
  if(window.isOpen()) {
    window.clear();

    for(const auto& context: contexts) {
      for(const auto& layer: context.layers) {
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
        } // for each Component in componentList
      } // for each RenderLayer in RenderContext
    } // for each RenderContext

    window.display();
  }
}

void farcical::engine::RenderSystem::Stop() {
  if(window.isOpen()) {
    window.close();
    WriteToLog("RenderSystem successfully shut down.");
  }
}

sf::RenderWindow& farcical::engine::RenderSystem::GetWindow() const {
  return const_cast<sf::RenderWindow&>(window);
}

std::expected<farcical::engine::RenderContext*, farcical::engine::Error>
farcical::engine::RenderSystem::CreateRenderContext(engine::EntityID sceneID) {
  RenderContext* existingContext{GetRenderContext(sceneID)};
  if(existingContext) {
    const auto& failMsg{
      "Invalid Configuration: Attempted to create a RenderContext with sceneID=\"" + sceneID +
      "\", but a RenderContext with that ID already exists!"
    };
    return std::unexpected(engine::Error{Error::Signal::InvalidConfiguration, failMsg});
  } // if this RenderContext already exists
  RenderContext& context{contexts.emplace_back(RenderContext{sceneID})};
  for(int index = 0; index < context.layers.size(); ++index) {
    context.layers[index].id = static_cast<ui::Layout::Layer::ID>(index);
  } // assign each Layer its correct LayerID
  return &context;
}

std::optional<farcical::engine::Error> farcical::engine::RenderSystem::DestroyRenderContext(engine::EntityID sceneID) {
  // Loop through each RenderContext to find the one in question
  for(auto contextIter = contexts.begin(); contextIter != contexts.end(); ++contextIter) {
    // If we find it, loop through each of its RenderLayers and remove all RenderComponents therein
    if(contextIter->sceneID == sceneID) {
      // Remove RenderComponents before erasing
      for(const auto& layer: contextIter->layers) {
        for(const auto& component: layer.componentList) {
          DestroyRenderComponent(sceneID, component->parentID);
        } // for each RenderComponent* in layer.componentList
      } // for each RenderLayer in (*contextIter).layers

      contexts.erase(contextIter);
      return std::nullopt;
    } // if (*contextIter) is the RenderContext we're meant to destroy
  } // for each RenderContext in contexts

  const auto& failMsg{
    "Invalid Configuration: Attempted to destroy a RenderContext with sceneID=\"" + sceneID +
    "\", but no such RenderContext exists!"
  };
  return engine::Error{Error::Signal::InvalidConfiguration, failMsg};
}

farcical::engine::RenderContext* farcical::engine::RenderSystem::GetRenderContext(
  engine::EntityID sceneID) const {
  RenderContext* contextPtr{nullptr};
  for(const auto& context: contexts) {
    if(context.sceneID == sceneID) {
      contextPtr = const_cast<RenderContext*>(&context);
      break;
    } // if context.sceneID == sceneID
  } // for each RenderContext in contexts
  return contextPtr;
}

std::expected<farcical::engine::RenderComponent*, farcical::engine::Error>
farcical::engine::RenderSystem::CreateRenderComponent(
  ui::Layout::Layer::ID layerID,
  EntityID sceneID,
  EntityID parentID,
  sf::Texture* texture) {
  RenderContext* context{GetRenderContext(sceneID)};
  if(!context) {
    const std::string failMsg{"Invalid configuration: RenderContext with id=\"" + sceneID + "\" not found."};
    return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
  } // if RenderContext not found

  const auto& createComponent{
    components.insert(std::make_pair(parentID, std::make_unique<RenderComponent>(parentID)))
  };
  if(createComponent.second) {
    RenderComponent* component{createComponent.first->second.get()};
    component->texture = texture;
    context->layers[static_cast<int>(layerID)].componentList.push_back(component);
    return component;
  } // if createComponent == success

  const std::string failMsg{"Invalid configuration: Failed to create RenderComponent for " + parentID + "."};
  return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
}

std::expected<farcical::engine::RenderComponent*, farcical::engine::Error>
farcical::engine::RenderSystem::CreateRenderComponent(
  ui::Layout::Layer::ID layerID,
  EntityID sceneID,
  EntityID parentID,
  sf::Font* font,
  const FontProperties& fontProperties,
  std::string_view contents) {
  RenderContext* context{GetRenderContext(sceneID)};
  if(!context) {
    const std::string failMsg{"Invalid configuration: RenderContext with id=\"" + sceneID + "\" not found."};
    return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
  } // if RenderContext not found

  const auto& createComponent{
    components.insert(std::make_pair(parentID, std::make_unique<RenderComponent>(parentID)))
  };
  if(createComponent.second) {
    RenderComponent* component{createComponent.first->second.get()};
    component->font = font;
    component->fontProperties = fontProperties;
    component->contents = contents;
    context->layers[static_cast<int>(layerID)].componentList.push_back(component);
    return component;
  } // if createComponent == success
  const std::string failMsg{"Invalid configuration: Failed to create RenderComponent for " + parentID + "."};
  return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
}

std::optional<farcical::engine::Error> farcical::engine::RenderSystem::DestroyRenderComponent(
  EntityID sceneID, EntityID parentID) {
  RenderContext* context{GetRenderContext(sceneID)};
  if(!context) {
    const std::string failMsg{"Invalid Configuration: Failed to find RenderContext with sceneID=\"" + sceneID + "\"."};
    return Error{Error::Signal::InvalidConfiguration, failMsg};
  }

  const auto& findComponent{components.find(parentID)};
  if(findComponent != components.end()) {
    for(int index = 0; index < static_cast<int>(ui::Layout::Layer::ID::NumLayers); ++index) {
      bool found{false};
      for(auto componentIter = context->layers[index].componentList.begin();
          !found && componentIter != context->layers[index].componentList.end(); ++componentIter) {
        if((*componentIter)->parentID == parentID) {
          context->layers[index].componentList.erase(componentIter);
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
