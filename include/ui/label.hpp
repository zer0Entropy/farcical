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

  class Label final : public Widget {
  public:
    explicit Label(engine::EntityID id, Container* parent = nullptr);
    ~Label() override = default;

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
/*
    static std::expected<Label*, engine::Error> Create(
                                                  engine::EntityID id,
                                                  sf::Font* font,
                                                  const FontProperties& properties,
                                                  Widget* parent) {
      if(!parent || !parent->IsContainer()) {
        const std::string failMsg{"Invalid configuration: Label with missing or invalid parent."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
      } // if missing or invalid parent
      if(!font) {
        const std::string failMsg{"Invalid configuration: Label with missing or invalid font."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
      }
      Container* container{dynamic_cast<Container*>(parent)};
      const unsigned int childIndex{container->GetNumChildren()};
      container->AddChild(std::make_unique<Label>(id, parent));
      Label* label = dynamic_cast<Label*>(container->GetChild(childIndex));
      label->SetFont(*font);
      label->SetFontProperties(properties);
      return label;
    }
*/
  private:
    sf::Font*       font;
    FontProperties  fontProperties;
    std::string     contents;
  };
}

#endif //LABEL_HPP
