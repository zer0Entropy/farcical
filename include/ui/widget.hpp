//
// Created by dgmuller on 5/30/25.
//
#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <SFML/System/Vector2.hpp>

#include "../engine/entity.hpp"
#include "action.hpp"

namespace farcical::ui {
  class Widget: public engine::Entity, public ActionHandler {
  public:
    enum class Type {
      Scene,
      Border,
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
                      Widget* parent = nullptr,
                      bool canReceiveFocus = false):
      Entity(id),
      ActionHandler(),
      type{type}, parent{parent}, canReceiveFocus{canReceiveFocus},
      size{0, 0}, scale{1.0f, 1.0f}, position{0.0f, 0.0f} {
    }

    [[nodiscard]] Type GetType() const { return type; }
    [[nodiscard]] Widget* GetParent() const { return parent; }
    [[nodiscard]] virtual sf::Vector2u GetSize() const { return size; }
    [[nodiscard]] sf::Vector2f GetScale() const { return scale; }
    [[nodiscard]] sf::Vector2f GetPosition() const { return position; }

    void SetSize(sf::Vector2u size) { this->size = size; }
    void SetScale(sf::Vector2f scale) { this->scale = scale; }
    void SetPosition(sf::Vector2f position) { this->position = position; }

    [[nodiscard]] bool CanReceiveFocus() const { return canReceiveFocus; }

    [[nodiscard]] virtual bool IsContainer() const { return false; }

    virtual void DoAction(Action action) = 0;

  protected:
    Type type;
    Widget* parent;
    bool canReceiveFocus;

    sf::Vector2u size;
    sf::Vector2f scale;
    sf::Vector2f position;
  };
}  // namespace farcical::ui

#endif //WIDGET_HPP
