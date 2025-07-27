//
// Created by dgmuller on 5/30/25.
//

#ifndef MENU_HPP
#define MENU_HPP

#include "label.hpp"
#include "button.hpp"
#include "container.hpp"
#include "../engine/event.hpp"
#include "../keyboard.hpp"
#include "../mouse.hpp"

namespace farcical::ui {
    class Button;
    class Label;

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

        [[nodiscard]] Label* GetLabel() const;

        void DoAction(Action action) override;
/*
        static std::expected<MenuItem*, engine::Error> Create(
            engine::EntityID id,
            std::string_view contents,
            sf::Font* font,
            const FontProperties& fontProperties,
            const std::vector<sf::Texture*>& textures,
            engine::Event::Type eventType,
            const std::vector<std::any>& eventArgs,
            Widget* parent) {
            if(!parent || !parent->IsContainer()) {
                const std::string failMsg{
                    "Invalid configuration: Label (id=" + id + ") with missing or invalid parent."
                };
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            } // if missing or invalid parent
            if(!font) {
                const std::string failMsg{"Invalid configuration: Missing or invalid font (id=" + id + ")."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            }
            Container* container{dynamic_cast<Container*>(parent)};
            const unsigned int childIndex{container->GetNumChildren()};
            container->AddChild(std::make_unique<MenuItem>(id, eventType, eventArgs, parent));
            MenuItem* item = dynamic_cast<MenuItem*>(container->GetChild(childIndex));
            const engine::EntityID buttonID{std::string{id} + "Button"};
            const engine::EntityID labelID{std::string{id} + "Label"};
            Button* button{item->CreateButton(buttonID, textures)};
            Label* label{item->CreateLabel(labelID, contents, fontProperties, *font)};
            return item;
        }
*/
    private:
        Button* button;
        Label* label;
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

        /*
                static std::expected<Menu*, engine::Error> Create(
                    engine::EntityID id,
                    Widget* parent,
                    sf::Font* font,
                    const FontProperties& fontProperties,
                    const std::vector<sf::Texture*>& buttonTextures,
                    const MenuItemCollection& items,
                    const MenuLayout& layout) {
                    if(!parent || !parent->IsContainer()) {
                        const std::string failMsg{"Invalid configuration: Menu with missing or invalid parent."};
                        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
                    } // if missing or invalid parent

                    if(!font) {
                        const std::string failMsg{"Unexpected nullptr: font."};
                        return std::unexpected(engine::Error{engine::Error::Signal::NullPtr, failMsg});
                    } // if !font

                    // Create the Menu as a child of its parent
                    Container* container{dynamic_cast<Container*>(parent)};
                    const unsigned int childIndex{container->GetNumChildren()};
                    container->AddChild(std::make_unique<Menu>(id, layout, parent));
                    Menu* menu = dynamic_cast<Menu*>(container->GetChild(childIndex));

                    // Create each MenuItem
                    for(unsigned int index = 0; index < items.ids.size(); ++index) {
                        const std::string itemID{items.ids[index]};
                        const std::string contents{items.contents[index]};
                        const engine::Event::Type eventType{items.eventTypes[index]};
                        const std::vector<std::any>& eventArgs{items.eventArgs[index]};
                        const auto& createMenuItem{
                            MenuItem::Create(
                                itemID,
                                contents,
                                font,
                                fontProperties,
                                buttonTextures,
                                eventType,
                                eventArgs,
                                menu)
                        };
                        if(createMenuItem.has_value()) {
                            MenuItem* menuItem{createMenuItem.value()};
                        } // if createMenuItem == success
                    }
                    return menu;
                }
                */
        /*
        static std::expected<Menu*, engine::Error> Create(
            engine::EntityID id,
            sf::Font* labelFont,
            const FontProperties& fontProperties,
            const std::vector<sf::Texture*>& buttonTextures,
            const std::vector<engine::EntityID>& itemIDs,
            const std::vector<std::string>& itemContents,
            const std::vector<engine::Event::Type>& itemEventTypes,
            const std::vector<std::vector<std::any> >& itemEventArgs,
            Widget* parent) {
            if(!parent || !parent->IsContainer()) {
                const std::string failMsg{"Invalid configuration: Label with missing or invalid parent."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            } // if missing or invalid parent
            if(!labelFont) {
                const std::string failMsg{"Invalid configuration: Label with missing or invalid font."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            }
            Container* container{dynamic_cast<Container*>(parent)};
            const unsigned int childIndex{container->GetNumChildren()};
            container->AddChild(std::make_unique<Menu>(id, parent));
            Menu* menu = dynamic_cast<Menu*>(container->GetChild(childIndex));
            for(unsigned int index = 0; index < itemIDs.size(); ++index) {
                std::string itemID{itemIDs.at(index)};
                std::string contents{itemContents.at(index)};
                engine::Event::Type activationEventType{itemEventTypes.at(index)};
                const std::vector<std::any>& activationEventArgs{itemEventArgs.at(index)};
                const auto& createMenuItem{
                    MenuItem::Create(
                        itemID,
                        contents,
                        labelFont,
                        fontProperties,
                        buttonTextures,
                        activationEventType,
                        activationEventArgs,
                        menu)
                };
                if(createMenuItem.has_value()) {
                    MenuItem* menuItem{createMenuItem.value()};
                } // if createMenuItem == success
            } // for each name in itemNames
            return menu;
        }
        */

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
