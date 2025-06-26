//
// Created by dgmuller on 5/30/25.
//

#ifndef LABEL_HPP
#define LABEL_HPP
#include <SFML/Graphics/Font.hpp>

#include "SFML/Graphics/Color.hpp"
#include "widget.hpp"
#include "../resource/resource.hpp"

namespace farcical::ui {

  struct TextProperties {
    std::string   contents;
    ResourceID    fontID;
    unsigned int  fontSize;
    sf::Color     fontColor;
    sf::Color     outlineColor;
    float         outlineThickness;
  };

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

    [[nodiscard]] const TextProperties& GetTextProperties() const;
    [[nodiscard]] sf::Vector2u GetSize() const override;

    void Draw(sf::RenderTarget& target) const override;

    void DoAction(Action action) override;

  private:
    sf::Font*       font;
    TextProperties  properties;
  };

}

#endif //LABEL_HPP
