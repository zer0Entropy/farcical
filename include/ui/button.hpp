//
// Created by dgmuller on 5/30/25.
//

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics/Texture.hpp>
#include "widget.hpp"
#include "../engine/system/event.hpp"
#include "../engine/keyboard.hpp"
#include "../engine/mouse.hpp"

namespace farcical::ui {
    class Button final : public Widget {
    public:
        enum class Status {
            Normal = 0,
            Highlighted,
            Pressed,
            Disabled,
            NumStates
        };

        class Controller final : public KeyboardInterface, public MouseInterface {
        public:
            Controller() = delete;

            explicit Controller(Button* button, engine::EventSystem& eventSystem);

            Controller(const Controller&) = delete;

            Controller(Controller&) = delete;

            Controller(Controller&&) = delete;

            ~Controller() override = default;

            void ReceiveMouseMovement(sf::Vector2i position) override;

            void ReceiveMouseButtonPress(sf::Mouse::Button mouseButton, sf::Vector2i position) override;

            void ReceiveMouseButtonRelease(sf::Mouse::Button mouseButton, sf::Vector2i position) override;

            void ReceiveKeyboardInput(sf::Keyboard::Key input) override;

        private:
            Button* button;
            engine::EventSystem& eventSystem;
        };

        explicit Button(engine::EntityID id,
                        const engine::Event::Parameters& onPress,
                        engine::EventSystem& eventSystem,
                        Container* parent);

        ~Button() override = default;

        void SetStatus(Status status);

        void SetTexture(Status status, sf::Texture& texture);

        [[nodiscard]] Status GetStatus() const;

        [[nodiscard]] sf::Texture* GetTexture(Status status) const;

        [[nodiscard]] sf::Texture* GetTexture() const;

        [[nodiscard]] const engine::Event::Parameters& GetOnPressEvent() const;

        [[nodiscard]] Button::Controller* GetController() const;

        void DoAction(Action action) override;

        static constexpr std::string_view GetStatusName(Status status) {
            std::string_view name{"?"};
            switch(status) {
                case Status::Normal: {
                    name = "normal";
                }
                break;
                case Status::Highlighted: {
                    name = "highlighted";
                }
                break;
                case Status::Pressed: {
                    name = "pressed";
                }
                break;
                case Status::Disabled: {
                    name = "disabled";
                }
                break;
                case Status::NumStates: {
                    name = "?";
                }
                break;
            } // switch(status)
            return name;
        }

        static constexpr Status GetStatusByName(std::string_view name) {
            Status status{Status::NumStates};
            if(name == "normal") {
                status = Status::Normal;
            } // if normal
            else if(name == "highlighted") {
                status = Status::Highlighted;
            } // else if highlighted
            else if(name == "pressed") {
                status = Status::Pressed;
            } // else if pressed
            else if(name == "disabled") {
                status = Status::Disabled;
            } // else if disabled
            return status;
        }

    private:
        sf::Texture* textures[static_cast<int>(Status::NumStates)];
        Status status;
        engine::Event::Parameters onPressEvent;
        std::unique_ptr<Button::Controller> controller;
    };
}

#endif //BUTTON_HPP