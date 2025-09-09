//
// Created by dgmuller on 5/24/25.
//

#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include <vector>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "../component/render.hpp"
#include "log.hpp"
#include "system.hpp"
#include "../../ui/layout.hpp"
#include "../../resource/config.hpp"

namespace farcical::engine {
    struct RenderLayer {
        ui::Layout::Layer::ID id;
        std::vector<RenderComponent*> componentList;

        void Add(RenderComponent* component) {
            componentList.push_back(component);
        }

        void Remove(EntityID parentID) {
            for(auto componentIter = componentList.begin();
                componentIter != componentList.end(); ++componentIter) {
                if((*componentIter)->parentID == parentID) {
                    componentList.erase(componentIter);
                    return;
                } // if IDs match
            } // for each component in componentList
        }

        void Clear() {
            componentList.clear();
        }
    };

    struct RenderContext {
        engine::EntityID sceneID;
        std::array<RenderLayer, static_cast<int>(ui::Layout::Layer::ID::NumLayers)> layers;
    };

    class RenderSystem final : public System {
    public:
        RenderSystem() = delete;

        RenderSystem(RenderSystem&) = delete;

        RenderSystem(const RenderSystem&) = delete;

        explicit RenderSystem(sf::RenderWindow& window, LogSystem& logSystem, ErrorGenerator* errorGenerator);

        ~RenderSystem() override = default;

        void Init() override;

        void Update() override;

        void Stop() override;

        sf::RenderWindow& GetWindow() const;

        // Create RenderContext to encapsulate RenderLayers
        std::expected<RenderContext*, Error> CreateRenderContext(engine::EntityID sceneID);

        // Destroy RenderContext and all RenderComponents it contains
        std::optional<Error> DestroyRenderContext(engine::EntityID sceneID);

        RenderContext* GetRenderContext(engine::EntityID sceneID) const;

        // Create RenderComponent for Sprite
        std::expected<RenderComponent*, Error> CreateRenderComponent(
            ui::Layout::Layer::ID layerID,
            EntityID sceneID,
            EntityID parentID,
            sf::Texture* texture);

        // Create RenderComponent for Text
        std::expected<RenderComponent*, Error> CreateRenderComponent(
            ui::Layout::Layer::ID layerID,
            EntityID sceneID,
            EntityID parentID,
            sf::Font* font,
            const FontProperties& fontProperties,
            std::string_view contents);

        std::optional<Error> DestroyRenderComponent(EntityID sceneID, EntityID parentID);

    private:
        sf::RenderWindow& window;
        std::vector<RenderContext> contexts;
        std::unordered_map<EntityID, std::unique_ptr<RenderComponent> > components;
    };
};

#endif //RENDER_SYSTEM_HPP