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
#include "../mouse.hpp"

namespace farcical {
    struct Resource;
    class ResourceManager;
}

namespace farcical::ui {
    class Scene;
    class Menu;
    class Decoration;

    /* ui::Manager creates and stores all ui::Widgets.
     *  Widgets are stored in a hierarchy, with one Widget (whichever was created first)
     *  designated as the root node.
    */

    class Manager final :
            public KeyboardInterface,
            public MouseInterface,
            public ActionHandler,
            public EventPropagator {
    public:
        explicit Manager(EventSystem& eventSystem);

        Manager(const Manager&) = delete;

        Manager(Manager&) = delete;

        Manager(Manager&&) = delete;

        ~Manager() override = default;

        std::optional<Error> Init(farcical::ResourceManager& resourceManager);

        Scene* CreateScene(std::string_view name);

        Menu* CreateMenu(std::string_view name, ResourceManager& resourceManager, Widget* parent = nullptr);

        Decoration* CreateDecoration(std::string_view name, std::string_view textureID, ResourceManager& resourceManager, Widget* parent = nullptr);

        [[nodiscard]] Widget* GetRootWidget() const;

        [[nodiscard]] Widget* GetFocusedWidget() const;

        [[nodiscard]] Widget* FindWidget(std::string_view name, Widget* parent = nullptr) const;

        void SetFocusedWidget(Widget* widget);

        void Update(sf::RenderWindow& window) const;

        void ReceiveKeyboardInput(sf::Keyboard::Key input) override;

        void ReceiveMouseMovement(sf::Vector2i position) override;

        void ReceiveMouseButtonClick(sf::Mouse::Button button, sf::Vector2i position) override;

        void DoAction(Action action) override;

        void ReceiveEvent(const Event& event) override;

    private:
        EventSystem& eventSystem;

        Config config;

        std::unique_ptr<Widget> rootWidget;
        Widget* focusedWidget;

        Resource* buttonTextures[3];
        Resource* buttonFont;

        static constexpr std::string_view buttonTextureNormalID{"buttonNormalTexture"};
        static constexpr std::string_view buttonTextureHighlightedID{"buttonHighlightedTexture"};
        static constexpr std::string_view buttonTexturePressedID{"buttonPressedTexture"};
        static constexpr std::string_view   buttonFontID{"buttonFont"};

        unsigned int defaultFontSize;
        sf::Color defaultFontColor;
        sf::Color                           defaultOutlineColor;
        float defaultOutlineThickness;
        float defaultButtonSpacing;
    };
}


#endif // UI_MANAGER_HPP
