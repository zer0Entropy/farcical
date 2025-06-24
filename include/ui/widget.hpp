//
// Created by dgmuller on 5/30/25.
//
#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

#include "action.hpp"
#include "../event.hpp"

namespace farcical::ui {
  class Widget : public ActionHandler, public EventPropagator {
  public:
    enum class Type {
      Border,
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

    explicit Widget(std::string_view name, Type type, Widget* parent = nullptr):
      ActionHandler(),
      EventPropagator(parent),
      name{name}, type{type}, parent{parent},
      size{0,0}, scale{1.0f, 1.0f}, position{0.0f, 0.0f} {}

    [[nodiscard]] std::string_view GetName() const { return name; }
    [[nodiscard]] Type GetType() const { return type; }
    [[nodiscard]] Widget* GetParent() const { return parent; }
    [[nodiscard]] virtual sf::Vector2u GetSize() const { return size; }
    [[nodiscard]] sf::Vector2f GetScale() const { return scale; }
    [[nodiscard]] sf::Vector2f GetPosition() const { return position; }

    void SetSize(sf::Vector2u size) { this->size = size; }
    void SetScale(sf::Vector2f scale) { this->scale = scale; }
    void SetPosition(sf::Vector2f position) { this->position = position; }

    [[nodiscard]] virtual bool IsContainer() const { return false; }

    virtual void Draw(sf::RenderTarget& target) const = 0;

    virtual void DoAction(Action action) = 0;

  protected:
    std::string name;
    Type type;
    Widget* parent;

    sf::Vector2u size;
    sf::Vector2f scale;
    sf::Vector2f position;
  };
}  // namespace farcical::ui

#endif //WIDGET_HPP
