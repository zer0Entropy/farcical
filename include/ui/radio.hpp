//
// Created by dgmuller on 7/27/25.
//

#ifndef RADIO_HPP
#define RADIO_HPP

#include <SFML/Graphics/Texture.hpp>
#include "widget.hpp"
#include "focusable.hpp"
#include "../engine/keyboard.hpp"
#include "../engine/mouse.hpp"
#include "../engine/system/event.hpp"

namespace farcical::ui {
    class RadioButton final : public Widget, public Focusable {
    public:
        enum class Status {
            Off = 0,
            OffHasFocus,
            On,
            OnHasFocus,
            NumStates
        };

        class Controller final : public KeyboardInterface, public MouseInterface {
        public:
            Controller() = delete;

            explicit Controller(RadioButton* radioButton, engine::EventSystem& eventSystem);

            Controller(const Controller&) = delete;

            Controller(Controller&) = delete;

            Controller(Controller&&) = delete;

            ~Controller() override = default;

            void ReceiveMouseMovement(sf::Vector2i position) override;

            void ReceiveMouseButtonPress(sf::Mouse::Button mouseButton, sf::Vector2i position) override;

            void ReceiveMouseButtonRelease(sf::Mouse::Button mouseButton, sf::Vector2i position) override;

            void ReceiveKeyboardInput(sf::Keyboard::Key input) override;

        private:
            RadioButton* radioButton;
            engine::EventSystem& eventSystem;
        };

        explicit RadioButton(engine::EntityID id, engine::EventSystem& eventSystem, Container* parent);

        ~RadioButton() override = default;

        void SetTexture(Status state, sf::Texture& texture);

        [[nodiscard]] sf::Texture* GetTexture(Status state) const;

        [[nodiscard]] sf::Texture* GetTexture() const;

        [[nodiscard]] Status GetStatus() const;

        void SetStatus(Status status);

        void DoAction(Action action) override;

        void OnReceiveFocus() override;

        void OnLoseFocus() override;

        [[nodiscard]] RadioButton::Controller* GetController() const;

        static constexpr std::string_view GetStatusName(Status status) {
            std::string_view name{"?"};
            switch(status) {
                case Status::Off: {
                    name = "off";
                }
                break;
                case Status::OffHasFocus: {
                    name = "offHasFocus";
                }
                break;
                case Status::On: {
                    name = "on";
                }
                break;
                case Status::OnHasFocus: {
                    name = "onHasFocus";
                }
                break;
                case Status::NumStates: {
                    name = "?";
                }
                break;
            }
            return name;
        }

        static constexpr Status GetStatusByName(std::string_view name) {
            Status status{Status::NumStates};
            if(name == "off") {
                status = Status::Off;
            } // off
            else if(name == "offHasFocus") {
                status = Status::OffHasFocus;
            } // offHasFocus
            else if(name == "on") {
                status = Status::On;
            } // on
            else if(name == "onHasFocus") {
                status = Status::OnHasFocus;
            } // on
            return status;
        }

    private:
        sf::Texture* textures[static_cast<int>(Status::NumStates)];
        Status status;
        std::unique_ptr<RadioButton::Controller> controller;
    };
}

#endif //RADIO_HPP