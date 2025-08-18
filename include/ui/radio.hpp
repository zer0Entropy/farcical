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

        static constexpr std::string_view GetStatusName(Status status) {
            std::string_view name{"?"};
            switch(status) {
                case Status::Unselected: {
                    name = "unselected";
                }
                break;
                case Status::Selected: {
                    name = "selected";
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
            if(name == "unselected") {
                status = Status::Unselected;
            } // unselected
            else if(name == "selected") {
                status = Status::Selected;
            } // selected
            return status;
        }

    private:
        sf::Texture* textures[static_cast<int>(Status::NumStates)];
        Status status;
    };
}

#endif //RADIO_HPP