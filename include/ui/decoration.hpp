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

        static std::expected<Decoration*, engine::Error> Create(engine::EntityID id,
                                                                sf::Texture* texture,
                                                                Container* parent) {
            if(!parent || !parent->IsContainer()) {
                const std::string failMsg{"Invalid configuration: Decoration with missing or invalid parent."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            } // if missing or invalid parent
            if(!texture) {
                const std::string failMsg{"Invalid configuration: Decoration with missing or invalid texture."};
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            }
            const unsigned int childIndex{parent->GetNumChildren()};
            parent->AddChild(std::make_unique<Decoration>(id, parent));
            Decoration* decoration = dynamic_cast<Decoration*>(parent->GetChild(childIndex));
            decoration->SetTexture(texture);
            return decoration;
        }

    private:
        sf::Texture* texture;
    };
}

#endif //DECORATION_HPP
