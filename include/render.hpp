//
// Created by dgmuller on 5/24/25.
//

#ifndef RENDER_HPP
#define RENDER_HPP

#include <vector>
#include "SFML/Graphics/RenderWindow.hpp"
#include "system.hpp"

namespace farcical {

    namespace ui {
      class Manager;
    }

    enum class RenderLayerID {
        Background  = 0,
        Foreground  = 1,
        Overlay     = 2,
        NumRenderLayers
    };

    struct RenderLayer {
        RenderLayerID id;
        std::vector<sf::Drawable*> drawables;
    };

    class RenderSystem final: public System
    {
    public:
        RenderSystem() = delete;
        RenderSystem(RenderSystem&) = delete;
        RenderSystem(const RenderSystem&) = delete;
        explicit RenderSystem(sf::RenderWindow& window, ui::Manager& manager);
        ~RenderSystem() override = default;

        void        Init() override;
        void        Update() override;
        void        Stop() override;

        void        AddToLayer(sf::Drawable* drawable, RenderLayerID layerID);
        void        RemoveFromLayer(sf::Drawable* drawable, RenderLayerID layerID);

    private:
        sf::RenderWindow&     window;
        ui::Manager&          uiManager;
        RenderLayer           layers[static_cast<int>(RenderLayerID::NumRenderLayers)];
    };

};

#endif //RENDER_HPP
