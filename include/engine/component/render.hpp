//
// Created by dgmuller on 8/11/25.
//

#ifndef RENDER_COMPONENT_HPP
#define RENDER_COMPONENT_HPP

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include "component.hpp"
#include "../../resource/config.hpp"

namespace farcical::engine {
    struct RenderComponent final : public Component {
        sf::Texture* texture;
        sf::Font* font;
        FontProperties fontProperties;
        std::string contents;
        sf::Vector2f position;
        sf::Vector2f scale;

        RenderComponent() = delete;

        explicit RenderComponent(EntityID parentID) : Component(Type::Render, parentID),
                                                      texture{nullptr},
                                                      font{nullptr},
                                                      contents{""},
                                                      position{0.0f, 0.0f},
                                                      scale{1.0f, 1.0f} {
        }

        ~RenderComponent() override = default;
    };
}

#endif //RENDER_COMPONENT_HPP