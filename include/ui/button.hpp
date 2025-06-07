//
// Created by dgmuller on 5/30/25.
//

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics/Texture.hpp>
#include "widget.hpp"

namespace farcical::ui {

  class Button final : public Widget {
  public:
    explicit Button(std::string_view name, Widget* parent);
    ~Button() override = default;

    static sf::Texture CreateTexture(   sf::Texture &leftTexture,
                                        sf::Texture &centerTexture,
                                        sf::Texture &rightTexture) {
      sf::Vector2u textureSize{
        leftTexture.getSize().x + centerTexture.getSize().x + rightTexture.getSize().x,
        leftTexture.getSize().y};
      sf::Texture texture{textureSize};
      texture.update(leftTexture, sf::Vector2u{0, 0});
      texture.update(centerTexture, sf::Vector2u{leftTexture.getSize().x, 0});
      texture.update(rightTexture, sf::Vector2u{leftTexture.getSize().x + centerTexture.getSize().x, 0});
      return texture;
    }

    void SetTexture(sf::Texture& texture);

    [[nodiscard]] sf::Vector2u GetSize() const override;

    void Draw(sf::RenderTarget& target) const override;

  private:
    sf::Texture* leftTexture;
    sf::Texture* centerTexture;
    sf::Texture* rightTexture;
    sf::Texture* texture;
  };

}

#endif //BUTTON_HPP
