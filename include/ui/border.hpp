//
// Created by dgmuller on 5/30/25.
//

#ifndef BORDER_HPP
#define BORDER_HPP

#include <SFML/Graphics/Texture.hpp>
#include "../geometry.hpp"
#include "widget.hpp"

namespace farcical::ui {
  class Border final: public Widget {
  public:
    explicit Border(EntityID id, Widget* parent);
    ~Border() override = default;
    sf::Texture   CreateTexture();

    void          SetCornerTexture(Rectangle::Corner corner, sf::Texture& texture);
    void          SetEdgeTexture(Rectangle::Edge edge, sf::Texture& texture);
    void          SetCenterTexture(sf::Texture& texture);

    void          DoAction(Action action) override;

  private:
    sf::Texture*  cornerTextures[static_cast<int>(Rectangle::Corner::NumCorners)];
    sf::Texture*  edgeTextures[static_cast<int>(Rectangle::Edge::NumEdges)];
    sf::Texture*  centerTexture;
  };
}

#endif //BORDER_HPP
