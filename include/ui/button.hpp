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

    explicit Button(engine::EntityID id, Container* parent);

    ~Button() override = default;

    void SetTexture(Status state, sf::Texture& texture);

    [[nodiscard]] sf::Texture* GetTexture(Status buttonStatus) const;

    [[nodiscard]] sf::Texture* GetTexture() const;

    [[nodiscard]] Status GetStatus() const;

    void SetStatus(Status status);

    void DoAction(Action action) override;

    static constexpr std::string_view GetStatusName(Status status) {
      std::string_view name{"?"};
      switch(status) {
        case Status::Normal: {
          name = "normal";
        }
        break;
        case Status::Highlighted: {
          name = "highlighted";
        }
        break;
        case Status::Pressed: {
          name = "pressed";
        }
        break;
        case Status::Disabled: {
          name = "disabled";
        }
        break;
        case Status::NumStates: {
          name = "?";
        }
        break;
      } // switch(status)
      return name;
    }

    static constexpr Status GetStatusByName(std::string_view name) {
      Status status{Status::NumStates};
      if(name == "normal") {
        status = Status::Normal;
      } // if normal
      else if(name == "highlighted") {
        status = Status::Highlighted;
      } // else if highlighted
      else if(name == "pressed") {
        status = Status::Pressed;
      } // else if pressed
      else if(name == "disabled") {
        status = Status::Disabled;
      } // else if disabled
      return status;
    }

  private:
    sf::Texture* textures[static_cast<int>(Status::NumStates)];
    Status status;
  };

}

#endif //BUTTON_HPP
