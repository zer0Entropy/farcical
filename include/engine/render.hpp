//
// Created by dgmuller on 5/24/25.
//

#ifndef RENDER_HPP
#define RENDER_HPP

#include <vector>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "component.hpp"
#include "system.hpp"
#include "../ui/layout.hpp"
#include "../resource/config.hpp"

namespace farcical::engine {
    struct RenderComponent final : public Component {
        sf::Texture* texture;
        sf::Font* font;
        FontProperties fontProperties;
        std::string contents;
        sf::Vector2f position;
        sf::Vector2f scale;

        RenderComponent() = delete;

        explicit RenderComponent(EntityID parentID): Component(Type::Render, parentID),
                                                     texture{nullptr},
                                                     font{nullptr},
                                                     contents{""},
                                                     position{0.0f, 0.0f},
                                                     scale{1.0f, 1.0f} {
        }

        ~RenderComponent() override = default;
    };

    struct RenderLayer {
        ui::Layout::Layer::ID id;
        std::vector<RenderComponent*> componentList;
    };

    class RenderSystem final : public System {
    public:
        RenderSystem() = delete;

        RenderSystem(RenderSystem&) = delete;

        RenderSystem(const RenderSystem&) = delete;

        explicit RenderSystem(sf::RenderWindow& window);

        ~RenderSystem() override = default;

        void Init() override;

        void Update() override;

        void Stop() override;

        // Create RenderComponent for Sprite
        std::expected<RenderComponent*, Error> CreateRenderComponent(ui::Layout::Layer::ID layerID, EntityID parentID,
                                                                     sf::Texture* texture);

        // Create RenderComponent for Text
        std::expected<RenderComponent*, Error> CreateRenderComponent(
            ui::Layout::Layer::ID layerID,
            EntityID parentID,
            sf::Font* font,
            const FontProperties& fontProperties,
            std::string_view contents);

        std::optional<Error> DestroyRenderComponent(EntityID parentID);

    private:
        sf::RenderWindow& window;
        RenderLayer layers[static_cast<int>(ui::Layout::Layer::ID::NumLayers)];
        std::unordered_map<EntityID, std::unique_ptr<RenderComponent> > components;
    };
};

#endif //RENDER_HPP
