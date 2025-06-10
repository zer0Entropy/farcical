//
// Created by dgmuller on 5/30/25.
//

#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics/Font.hpp>
#include "widget.hpp"

namespace farcical::ui {
    class Button;
    class Label;

    class MenuItem final : public Widget {
    public:
        explicit MenuItem(std::string_view name, Widget* parent = nullptr);

        ~MenuItem() override = default;

        Button* CreateButton(std::string_view name, sf::Texture& texture);

        Label* CreateLabel(std::string_view name, sf::Font& font);

        Button* GetButton() const;

        Label* GetLabel() const;

        void Draw(sf::RenderTarget& target) const override;

    private:
        Button* button;
        Label* label;

        static int fontSize;
        static sf::Color fontColor;
        static sf::Color outlineColor;
    };

    class Menu final : public Widget {
    public:
        explicit Menu(std::string_view name, Widget* parent = nullptr);

        ~Menu() override = default;

        [[nodiscard]] MenuItem* CreateMenuItem(std::string_view name);

        [[nodiscard]] MenuItem* GetMenuItemByName(std::string_view name) const;

        [[nodiscard]] MenuItem* GetMenuItemByIndex(int index) const;

        void SetButtonTexture(sf::Texture& texture);

        void SetLabelFont(sf::Font& font);

        void SetSelectedIndex(int index);

        void SetButtonSpacing(float spacing);

        void Draw(sf::RenderTarget& target) const override;

    private:
        sf::Texture* buttonTexture;
        sf::Font* labelFont;
        int selectedIndex;
        float buttonSpacing;
    };
}

#endif //MENU_HPP
