//
// Created by dgmuller on 5/30/25.
//

#ifndef LABEL_HPP
#define LABEL_HPP

#include <expected>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>

#include "container.hpp"
#include "../resource/config.hpp"
#include "../engine/error.hpp"

namespace farcical::ui {
  class Text final : public Widget {
  public:
    explicit Text(engine::EntityID id, Container* parent = nullptr);

    ~Text() override = default;

    void SetFont(sf::Font& font);
    void SetFontSize(unsigned int fontSize);
    void SetFontColor(sf::Color color);
    void SetOutlineColor(sf::Color color);
    void SetOutlineThickness(float thickness);
    void SetContents(std::string_view contents);

    void SetFontProperties(const FontProperties& properties);

    [[nodiscard]] std::string_view GetContents() const;

    [[nodiscard]] sf::Font* GetFont() const;

    [[nodiscard]] const FontProperties& GetFontProperties() const;

    void DoAction(Action action) override;

  private:
    sf::Font*       font;
    FontProperties  fontProperties;
    std::string     contents;
  };
}

#endif //LABEL_HPP
