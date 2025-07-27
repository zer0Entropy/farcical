//
// Created by dgmuller on 6/24/25.
//

#ifndef DECORATION_HPP
#define DECORATION_HPP

#include <expected>
#include <memory>

#include "container.hpp"
#include "../engine/error.hpp"

namespace farcical::ui {
    class Decoration final : public Widget {
    public:
        Decoration() = delete;
        Decoration(const Decoration&) = delete;
        Decoration(Decoration&) = delete;
        Decoration(Decoration&&) = delete;
        Decoration& operator=(const Decoration&) = delete;

        explicit Decoration(engine::EntityID id, Container* parent);

        ~Decoration() override = default;

        void DoAction(Action action) override;

        [[nodiscard]] sf::Texture* GetTexture() const;

        void SetTexture(sf::Texture* texture);

    private:
        sf::Texture* texture;
    };
}

#endif //DECORATION_HPP
