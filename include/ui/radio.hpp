//
// Created by dgmuller on 7/27/25.
//

#ifndef RADIO_HPP
#define RADIO_HPP

#include <SFML/Graphics/Texture.hpp>
#include "container.hpp"

namespace farcical::ui {
    class RadioButton final : public Widget {
    public:
        enum class Status {
            Unselected = 0,
            Selected,
            NumStates
        };

        explicit RadioButton(engine::EntityID id, Container* parent);

        ~RadioButton() override = default;

        void SetTexture(Status state, sf::Texture& texture);

        [[nodiscard]] sf::Texture* GetTexture(Status state) const;

        [[nodiscard]] sf::Texture* GetTexture() const;

        [[nodiscard]] Status GetStatus() const;

        void SetStatus(Status status);

        void DoAction(Action action) override;

    private:
        sf::Texture* textures[static_cast<int>(Status::NumStates)];
        Status status;
    };

    class RadioButtonGroup final : public Container {
    public:
        explicit RadioButtonGroup(engine::EntityID id, Container* parent);

        ~RadioButtonGroup() override = default;

        void AddChild(std::unique_ptr<Widget> child) override;

        RadioButton* GetSelectedButton() const;

        RadioButton* GetButtonByID(engine::EntityID id) const;

        RadioButton* GetButtonByIndex(int index) const;

        void SetSelectedButton(RadioButton* button);

        void DoAction(Action action) override;

    private:
        std::vector<RadioButton*> radioButtons;
        RadioButton* selectedButton;
    };
}

#endif //RADIO_HPP
