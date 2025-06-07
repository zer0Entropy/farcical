//
// Created by dgmuller on 5/30/25.
//

#ifndef LABEL_HPP
#define LABEL_HPP
#include <SFML/Graphics/Font.hpp>

#include "SFML/Graphics/Color.hpp"
#include "widget.hpp"

namespace farcical::ui {

  class Label final : public Widget {
  public:
    explicit Label(std::string_view name, Widget* parent = nullptr);
    ~Label() override = default;

    void SetFont(sf::Font& font);
    void SetFontSize(unsigned int fontSize);
    void SetFontColor(sf::Color color);
    void SetOutlineColor(sf::Color color);
    void SetOutlineThickness(float thickness);
    void SetContents(std::string_view contents);

    sf::Vector2u GetSize() const override;

    void Draw(sf::RenderTarget& target) const override;

  private:
    sf::Font*     font;
    unsigned int  fontSize;
    sf::Color     fontColor;
    sf::Color     outlineColor;
    float         outlineThickness;
    std::string   contents;
  };

}

#endif //LABEL_HPP
