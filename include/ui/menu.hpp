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

        explicit MenuItem(engine::EntityID id, engine::Event::Type onSelection, Widget* parent = nullptr);

        ~MenuItem() override = default;

        [[nodiscard]] Button* CreateButton(engine::EntityID id, std::vector<sf::Texture*> textures);

        [[nodiscard]] Label* CreateLabel(engine::EntityID id, std::string_view contents, const FontProperties& fontProperties, sf::Font& font);

        [[nodiscard]] engine::Event::Type OnSelection() const;

        [[nodiscard]] Button* GetButton() const;

        [[nodiscard]] Label* GetLabel() const;

        void DoAction(Action action) override;

        static std::expected<MenuItem*, engine::Error> Create(
            engine::EntityID id,
            std::string_view contents,
            sf::Font* font,
            const FontProperties& fontProperties,
            const std::vector<sf::Texture*>& textures,
            engine::Event::Type onSelection,
            Widget* parent) {
            if(!parent || !parent->IsContainer()) {
                const std::string failMsg{"Invalid configuration: Label (id=" + id + ") with missing or invalid parent."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            } // if missing or invalid parent
            if(!font) {
                const std::string failMsg{"Invalid configuration: Missing or invalid font (id=" + id + ")."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            }
            Container* container{dynamic_cast<Container*>(parent)};
            const unsigned int childIndex{container->GetNumChildren()};
            container->AddChild(std::make_unique<MenuItem>(id, onSelection, parent));
            MenuItem* item = dynamic_cast<MenuItem*>(container->GetChild(childIndex));
            const engine::EntityID buttonID{std::string{id} + "Button"};
            const engine::EntityID labelID{std::string{id} + "Label"};
            Button* button{item->CreateButton(buttonID, textures)};
            Label* label{item->CreateLabel(labelID, contents, fontProperties, *font)};
            return item;
        }

    private:
        Button* button;
        Label* label;
        engine::Event::Type triggeredOnSelection;
    };

    class Menu final : public Container {
    public:
        explicit Menu(engine::EntityID id, Widget* parent = nullptr);
        ~Menu() override = default;

        void AddChild(std::unique_ptr<Widget> child) override;

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

        void SetButtonSpacing(float spacing);

        void DoAction(Action action) override;

        static std::expected<Menu*, engine::Error> Create(
            engine::EntityID id,
            sf::Font* labelFont,
            const FontProperties& fontProperties,
            std::vector<sf::Texture*> buttonTextures,
            std::vector<engine::EntityID> itemIDs,
            std::vector<std::string> itemContents,
            std::vector<engine::Event::Type> itemEvents,
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
                engine::Event::Type onSelection{itemEvents.at(index)};
                const auto& createMenuItem{MenuItem::Create(itemID, contents, labelFont, fontProperties, buttonTextures, onSelection, menu)};
                if(createMenuItem.has_value()) {
                    MenuItem* menuItem{createMenuItem.value()};
                } // if createMenuItem == success
            } // for each name in itemNames
            return menu;
        }

    private:
        std::vector<MenuItem*> items;
        sf::Texture* buttonTextureNormal;
        sf::Texture* buttonTextureHighlighted;
        sf::Texture* buttonTexturePressed;
        sf::Font* labelFont;
        sf::Font* titleFont;
        int selectedIndex;
        float buttonSpacing;
    };

    class MenuController final: public MouseInterface, public KeyboardInterface {
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
