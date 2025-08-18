//
// Created by dgmuller on 5/30/25.
//
#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include "../engine/entity.hpp"
#include "action.hpp"

namespace farcical::ui {
  class Container;

  class Widget: public engine::Entity, public ActionHandler {
  public:
    enum class Type {
      Scene,
      Decoration,
      Menu,
      Button,
      RadioButton,
      Text
    };

    Widget() = delete;
    Widget(const Widget&) = delete;
    Widget(Widget&) = delete;
    Widget(Widget&&) = delete;
    virtual ~Widget() = default;
    Widget& operator=(const Widget&) = delete;

    explicit Widget(  engine::EntityID id,
                      Type type,
                      Container* parent = nullptr,
                      bool canReceiveFocus = false):
      Entity(id),
      ActionHandler(),
      type{type},
      parent{parent},
      canReceiveFocus{canReceiveFocus},
      isColorized{false},
      size{0, 0},
      scale{1.0f, 1.0f},
      position{0.0f, 0.0f} {
    }

    [[nodiscard]] Type GetType() const { return type; }
    [[nodiscard]] Container* GetParent() const { return parent; }
    [[nodiscard]] const sf::Vector2u& GetSize() const { return size; }
    [[nodiscard]] const sf::Vector2f& GetScale() const { return scale; }
    [[nodiscard]] const sf::Vector2f& GetPosition() const { return position; }

    [[nodiscard]] sf::IntRect GetBounds() const {
      return sf::IntRect{
        sf::Vector2i{static_cast<int>(position.x), static_cast<int>(position.y)},
        sf::Vector2i{static_cast<int>(size.x), static_cast<int>(size.y)}
      };
    }

    [[nodiscard]] const sf::Color& GetColor() const { return color; }
    [[nodiscard]] bool IsColorized() const { return isColorized; }

    void SetSize(const sf::Vector2u& size) { this->size = size; }
    void SetScale(const sf::Vector2f& scale) { this->scale = scale; }
    void SetPosition(const sf::Vector2f& position) { this->position = position; }

    void SetColor(const sf::Color& color) {
      this->color = color;
      isColorized = true;
    }

    [[nodiscard]] bool CanReceiveFocus() const { return canReceiveFocus; }

    [[nodiscard]] virtual bool IsContainer() const { return false; }

    virtual void DoAction(Action action) = 0;

    static constexpr std::string_view GetTypeName(Type type) {
      std::string_view typeName{"?"};
      switch(type) {
        case Type::Scene: {
          typeName = "Scene";
        }
        break;
        case Type::Decoration: {
          typeName = "Decoration";
        }
        break;
        case Type::Menu: {
          typeName = "Menu";
        }
        break;
        case Type::Button: {
          typeName = "Button";
        }
        break;
        case Type::RadioButton: {
          typeName = "RadioButton";
        }
        break;
        case Type::Text: {
          typeName = "Text";
        }
        break;
      }
      return typeName;
    }

    static constexpr Type GetTypeByName(std::string_view typeName) {
      Type type{Type::Scene};
      if(typeName == "scene" || typeName == "Scene") {
        type = Type::Scene;
      } // if RootContainer
      else if(typeName == "decoration" || typeName == "Decoration") {
        type = Type::Decoration;
      } // else if Decoration
      else if(typeName == "button" || typeName == "Button") {
        type = Type::Button;
      } // else if Button
      else if(typeName == "radioButton" || typeName == "RadioButton") {
        type = Type::RadioButton;
      } // else if RadioButton
      else if(typeName == "menu" || typeName == "Menu") {
        type = Type::Menu;
      } // else if Menu
      return type;
    }

  protected:
    Type type;
    Container* parent;
    bool canReceiveFocus;
    bool isColorized;

    sf::Vector2u size;
    sf::Vector2f scale;
    sf::Vector2f position;
    sf::Color color;
  };
}  // namespace farcical::ui

#endif //WIDGET_HPP
