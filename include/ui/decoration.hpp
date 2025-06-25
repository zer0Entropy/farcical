//
// Created by dgmuller on 6/24/25.
//

#ifndef DECORATION_HPP
#define DECORATION_HPP

#include "widget.hpp"

namespace farcical::ui {
    class Decoration final : public Widget {
    public:
        Decoration() = delete;

        Decoration(const Decoration&) = delete;

        Decoration(Decoration&) = delete;

        Decoration(Decoration&&) = delete;

        Decoration& operator=(const Decoration&) = delete;

        explicit Decoration(std::string_view name, Widget* parent);

        ~Decoration() override = default;

        void Draw(sf::RenderTarget& target) const override;

        void DoAction(Action action) override;

        void SetTexture(sf::Texture* texture);

    private:
        sf::Texture* texture;
    };
}

#endif //DECORATION_HPP
