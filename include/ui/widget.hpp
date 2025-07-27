//
// Created by dgmuller on 5/30/25.
//
#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "../engine/entity.hpp"
#include "action.hpp"

namespace farcical::ui {
  class Container;

  class Widget: public engine::Entity, public ActionHandler {
  public:
    enum class Type {
      RootContainer,
      Decoration,
      Menu,
      MenuItem,
      Button,
      Label
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
