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
    enum class Status {
      Normal = 0,
      Highlighted,
      Pressed,
      Disabled,
      NumStates
    };

    explicit Button(std::string_view name, Widget* parent);
    ~Button() override = default;

    void SetTexture(Status state, sf::Texture& texture);

    [[nodiscard]] sf::Vector2u GetSize() const override;

    [[nodiscard]] Status GetStatus() const;

    void SetStatus(Status status);

    void Draw(sf::RenderTarget& target) const override;

  private:
    sf::Texture* textures[static_cast<int>(Status::NumStates)];
    Status status;
  };

}

#endif //BUTTON_HPP
