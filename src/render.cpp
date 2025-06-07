//
// Created by dgmuller on 5/24/25.
//

#include "../include/render.hpp"

#include "../include/ui/manager.hpp"
#include "SFML/Graphics/Drawable.hpp"

farcical::RenderSystem::RenderSystem(sf::RenderWindow &window, ui::Manager& manager):
  System{System::ID::RenderSystem},
  window{window},
  uiManager{manager},
  layers{} {

}

void farcical::RenderSystem::Init() {

}

void farcical::RenderSystem::Update() {
  if(window.isOpen()) {
    window.clear();
    for(int layerIndex = static_cast<int>(RenderLayerID::Foreground); layerIndex < static_cast<int>(RenderLayerID::NumRenderLayers); ++layerIndex) {
      RenderLayer &layer{layers[layerIndex]};
      for(const auto& drawable : layer.drawables) {
        window.draw(*drawable);
      }
    }
    uiManager.Update(window);
    window.display();
  }
}

void farcical::RenderSystem::Stop() {
  if (window.isOpen()) {
    window.close();
  }
}

void farcical::RenderSystem::AddToLayer(sf::Drawable* drawable, RenderLayerID layerID) {
  layers[static_cast<int>(layerID)].drawables.push_back(drawable);
}

void farcical::RenderSystem::RemoveFromLayer(sf::Drawable* drawable, RenderLayerID layerID) {
  auto& layer = layers[static_cast<int>(layerID)];
  for(auto it = layer.drawables.begin(); it != layer.drawables.end(); ++it) {
    if(*it == drawable) {
      layer.drawables.erase(it);
      break;
    }
  }
}
