//
// Created by dgmuller on 5/30/25.
//

#ifndef MENU_HPP
#define MENU_HPP

#include <variant>
#include "text.hpp"
#include "button.hpp"
#include "container.hpp"
#include "radio.hpp"
#include "../engine/system/event.hpp"
#include "../engine/keyboard.hpp"
#include "../engine/mouse.hpp"

namespace farcical::ui {
    class Button;
    class Text;

    struct MenuLayout {
        Orientation orientation;
        int relativeSpacing;
        std::unordered_map<engine::EntityID, sf::Vector2f> positions;

        MenuLayout() : orientation{Orientation::Vertical},
                       relativeSpacing{0} {
        }

        explicit MenuLayout(Orientation orientation, int relativeSpacing) : orientation{orientation},
                                                                            relativeSpacing{relativeSpacing} {
        }

        ~MenuLayout() = default;

        constexpr static std::string_view GetOrientationName(Orientation orientation) {
            std::string_view name{"?"};
            switch(orientation) {
                case Orientation::Vertical: {
                    name = "vertical";
                }
                break;
                case Orientation::Horizontal: {
                    name = "horizontal";
                }
                break;
            } // switch(orientation)
            return name;
        }

        constexpr static Orientation GetOrientationByName(std::string_view name) {
            Orientation orientation{Orientation::Vertical};
            if(name == "vertical") {
                orientation = Orientation::Vertical;
            } // if vertical
            else if(name == "horizontal") {
                orientation = Orientation::Horizontal;
            } // else if horizontal
            return orientation;
        }
    };

    class Menu final : public Container {
    public:
        using Item = std::variant<Button*, RadioButton*, Menu*>;

        enum class Type {
            Button,
            RadioButton,
            SubMenu
        };

        explicit Menu(engine::EntityID id, Menu::Type type, const MenuLayout& layout, Container* parent = nullptr);

        ~Menu() override = default;

        void AddChild(std::unique_ptr<Widget> child) override;

        [[nodiscard]] Menu::Type GetMenuType() const;

        [[nodiscard]] sf::Texture* GetButtonTexture(Button::Status status) const;

        [[nodiscard]] sf::Texture* GetRadioButtonTexture(RadioButton::Status status) const;

        void AddButtonTexture(Button::Status status, sf::Texture& texture);

        void AddRadioButtonTexture(RadioButton::Status status, sf::Texture& texture);

        void SetLabelFont(sf::Font& font);

        void SetTitleFont(sf::Font& font);

        void DoAction(Action action) override;

        static constexpr std::string_view GetTypeName(Menu::Type type) {
            std::string_view typeName{"?"};
            switch(type) {
                case Menu::Type::Button: {
                    typeName = "button";
                }
                break;
                case Menu::Type::RadioButton: {
                    typeName = "radioButton";
                }
                break;
                case Menu::Type::SubMenu: {
                    typeName = "subMenu";
                }
                break;
            }
            return typeName;
        }

        static constexpr Menu::Type GetTypeByName(std::string_view name) {
            Menu::Type type;
            if(name == "button" || name == "Button") {
                type = Menu::Type::Button;
            } // if Button
            else if(name == "radioButton" || name == "RadioButton") {
                type = Menu::Type::RadioButton;
            } // else if RadioButton
            else if(name == "subMenu" || name == "SubMenu") {
                type = Menu::Type::SubMenu;
            } // else if SubMenu
            return type;
        }

    private:
        MenuLayout layout;
        Menu::Type menuType;

        std::vector<Button*> buttons;
        std::vector<RadioButton*> radioButtons;
        std::vector<Menu*> subMenus;

        std::map<Button::Status, sf::Texture*> buttonTextures;
        std::map<RadioButton::Status, sf::Texture*> radioButtonTextures;

        sf::Font* labelFont;
        sf::Font* titleFont;
    };

    /*
    class MenuController final : public engine::Entity, public MouseInterface, public KeyboardInterface {
    public:
        MenuController(Menu* menu, engine::EventSystem& eventSystem);

        ~MenuController() override = default;

        void ReceiveMouseMovement(sf::Vector2i position) override;

        void ReceiveMouseButtonPress(sf::Mouse::Button buttonPressed, sf::Vector2i position) override;

        void ReceiveMouseButtonRelease(sf::Mouse::Button buttonReleased, sf::Vector2i position) override;

        void ReceiveKeyboardInput(sf::Keyboard::Key input) override;

        static constexpr std::string_view MenuControllerName{"Controller"};

    private:
        Menu* menu;
        engine::EventSystem& eventSystem;
    };
*/
}

#endif //MENU_HPP