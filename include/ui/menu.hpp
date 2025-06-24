//
// Created by dgmuller on 5/30/25.
//

#ifndef MENU_HPP
#define MENU_HPP

#include <functional>
#include <SFML/Graphics/Font.hpp>

#include "button.hpp"
#include "container.hpp"

namespace farcical::ui {
    class Button;
    class Label;

    class MenuItem final : public Container {
    public:
        explicit MenuItem(std::string_view name, Widget* parent = nullptr);

        ~MenuItem() override = default;

        Button* CreateButton(std::string_view name, std::vector<sf::Texture*> textures);

        Label* CreateLabel(std::string_view name, sf::Font& font);

        Button* GetButton() const;

        Label* GetLabel() const;

        void DoAction(Action action) override;

    private:
        Button* button;
        Label* label;

        static int fontSize;
        static sf::Color fontColor;
        static sf::Color outlineColor;
    };

    class Menu final : public Container {
    public:
        explicit Menu(std::string_view name, Widget* parent = nullptr);

        ~Menu() override = default;

        [[nodiscard]] MenuItem* CreateMenuItem(std::string_view name);

        [[nodiscard]] MenuItem* GetMenuItemByName(std::string_view name) const;

        [[nodiscard]] MenuItem* GetMenuItemByIndex(int index) const;

        [[nodiscard]] MenuItem* GetMenuItemUnderCursor(sf::Vector2i position) const;

        [[nodiscard]] int GetSelectedIndex() const;

        void SetButtonTexture(Button::Status state, sf::Texture& texture);

        void SetLabelFont(sf::Font& font);

        void SetTitleFont(sf::Font& font);

        void SetSelectedIndex(int index);

        void SetButtonSpacing(float spacing);

        void DoAction(Action action) override;

    private:
        sf::Texture* buttonTextureNormal;
        sf::Texture* buttonTextureHighlighted;
        sf::Texture* buttonTexturePressed;
        sf::Font* labelFont;
        sf::Font* titleFont;
        int selectedIndex;
        float buttonSpacing;
    };
}

#endif //MENU_HPP
