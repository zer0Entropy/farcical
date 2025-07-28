//
// Created by dgmuller on 5/30/25.
//

#ifndef MENU_HPP
#define MENU_HPP

#include "text.hpp"
#include "button.hpp"
#include "container.hpp"
#include "../engine/event.hpp"
#include "../keyboard.hpp"
#include "../mouse.hpp"

namespace farcical::ui {
    class Button;
    class Text;

    class MenuItem final : public Container {
    public:
        class Menu;

        explicit MenuItem(engine::EntityID id,
                          engine::Event::Type activationEventType,
                          const std::vector<std::any>& activationEventArgs,
                          Container* parent = nullptr);

        ~MenuItem() override = default;

        void AddChild(std::unique_ptr<Widget> child) override;

        [[nodiscard]] engine::Event::Type GetActivationEventType() const;

        [[nodiscard]] const std::vector<std::any>& GetActivationEventArgs() const;

        [[nodiscard]] Button* GetButton() const;

        [[nodiscard]] Text* GetLabel() const;

        void DoAction(Action action) override;

    private:
        Button* button;
        Text* label;
        engine::Event::Type activationEventType;
        std::vector<std::any> activationEventArgs;
    };

    struct MenuLayout {
        enum class Orientation {
            Horizontal,
            Vertical
        };

        Orientation orientation;
        int relativeSpacing;
        float actualSpacing;
        std::unordered_map<engine::EntityID, sf::Vector2f> positions;

        MenuLayout(): orientation{Orientation::Vertical},
                      relativeSpacing{0},
                      actualSpacing{0.0f} {
        }

        explicit MenuLayout(Orientation orientation, int relativeSpacing): orientation{orientation},
                                                                           relativeSpacing{relativeSpacing},
                                                                           actualSpacing{0.0f} {
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

    struct MenuItemCollection {
        std::vector<engine::EntityID> ids;
        std::vector<std::string> contents;
        std::vector<engine::Event::Type> eventTypes;
        std::vector<std::vector<std::any> > eventArgs;
    };

    class Menu final : public Container {
    public:
        explicit Menu(engine::EntityID id, const MenuLayout& layout, Container* parent = nullptr);

        ~Menu() override = default;

        void AddChild(std::unique_ptr<Widget> child) override;

        [[nodiscard]] const std::vector<MenuItem*>& GetMenuItems() const;

        [[nodiscard]] int GetNumMenuItems() const;

        [[nodiscard]] MenuItem* GetMenuItemByID(engine::EntityID id) const;

        [[nodiscard]] MenuItem* GetMenuItemByIndex(int index) const;

        [[nodiscard]] MenuItem* GetMenuItemUnderCursor(sf::Vector2i position) const;

        [[nodiscard]] int GetSelectedIndex() const;

        [[nodiscard]] sf::Texture* GetButtonTexture(Button::Status state) const;

        void SetButtonTexture(Button::Status state, sf::Texture& texture);

        void SetLabelFont(sf::Font& font);

        void SetTitleFont(sf::Font& font);

        void SetSelectedIndex(int index);

        void DoAction(Action action) override;

    private:
        MenuLayout layout;
        std::vector<MenuItem*> items;
        sf::Texture* buttonTextureNormal;
        sf::Texture* buttonTextureHighlighted;
        sf::Texture* buttonTexturePressed;
        sf::Font* labelFont;
        sf::Font* titleFont;
        int selectedIndex;
    };

    class MenuController final : public MouseInterface, public KeyboardInterface {
    public:
        MenuController(Menu* menu, engine::EventSystem& eventSystem);

        ~MenuController() override = default;

        void ReceiveMouseMovement(sf::Vector2i position) override;

        void ReceiveMouseButtonPress(sf::Mouse::Button button, sf::Vector2i position) override;

        void ReceiveMouseButtonRelease(sf::Mouse::Button button, sf::Vector2i position) override;

        void ReceiveKeyboardInput(sf::Keyboard::Key input) override;

    private:
        Menu* menu;
        engine::EventSystem& eventSystem;
    };
}

#endif //MENU_HPP
