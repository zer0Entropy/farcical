//
// Created by dgmuller on 5/30/25.
//

#ifndef BORDER_HPP
#define BORDER_HPP

#include "../geometry.hpp"
#include "SFML/Graphics/Color.hpp"
#include "widget.hpp"

namespace farcical::ui {
  class Border final: public Widget {
  public:
    explicit Border(std::string_view name, Widget* parent);
    ~Border() override = default;
    sf::Texture   CreateTexture();

    void          SetCornerTexture(farcical::Rectangle::Corner corner, sf::Texture& texture);
    void          SetEdgeTexture(farcical::Rectangle::Edge edge, sf::Texture& texture);
    void          SetCenterTexture(sf::Texture& texture);

    void          Draw(sf::RenderTarget& target) const override;

  private:
    sf::Texture*  cornerTextures[static_cast<int>(farcical::Rectangle::Corner::NumCorners)];
    sf::Texture*  edgeTextures[static_cast<int>(farcical::Rectangle::Edge::NumEdges)];
    sf::Texture*  centerTexture;
  };
}

#endif //BORDER_HPP
