//
// Created by dgmuller on 5/30/25.
//

#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include <expected>
#include <memory>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../resource/config.hpp"
#include "widget.hpp"
#include "../keyboard.hpp"

namespace farcical {
  struct Resource;
  class ResourceManager;
}

namespace farcical::ui {
  class Menu;

  class Manager: public KeyboardInterface {
  public:
    explicit Manager();
    Manager(const Manager&) = delete;
    Manager(Manager&) = delete;
    Manager(Manager&&) = delete;
    ~Manager() override = default;

    std::optional<Error>  Init(farcical::ResourceManager& resourceManager);

    Menu*                 CreateMenu(std::string_view name, ResourceManager& resourceManager);

    [[nodiscard]] Widget* GetWidget(std::string_view name) const;
    [[nodiscard]] Widget* GetFocusedWidget() const;
    void                  SetFocusedWidget(Widget* widget);

    void                  Update(sf::RenderWindow& window) const;

    void                  ReceiveKeyboardInput(sf::Keyboard::Key input) override;

  private:

    Config                                              config;

    std::vector<std::unique_ptr<Widget>>                widgets;
    Widget*                                             focusedWidget;

    Resource*                                           buttonTexture;
    Resource*                                           buttonFont;

    static constexpr std::string_view                   buttonTextureID{"buttonNormalTexture"};
    static constexpr std::string_view                   buttonFontID{"buttonFont"};

    unsigned int                                        defaultFontSize;
    sf::Color                                           defaultFontColor;
    sf::Color                                           defaultOutlineColor;
    float                                               defaultOutlineThickness;
    float                                               defaultButtonSpacing;
  };
}


#endif //MANAGER_HPP
