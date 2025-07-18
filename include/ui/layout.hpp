//
// Created by dgmuller on 7/1/25.
//

#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include <array>
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace farcical::ui {

    class Widget;

    struct Layout {

        struct Layer {
            enum class ID {
                Background = 0,
                Foreground,
                Overlay,
                NumLayers
            };
            struct Node {
                Widget* widget;
                sf::Vector2u relativePosition;
            };

            ID id;
            std::vector<Node> nodes;

            static constexpr ID GetLayerIDByName(std::string_view name) {
                ID layerID{ID::NumLayers};
                if(name == "background" || name == "Background") {
                    layerID = ID::Background;
                }
                else if(name == "foreground" || name == "Foreground") {
                    layerID = ID::Foreground;
                }
                else if(name == "overlay" || name == "Overlay") {
                    layerID = ID::Overlay;
                }
                return layerID;
            }
        };

        std::array<Layer, static_cast<int>(Layer::ID::NumLayers)> layers;
    };

}

#endif //LAYOUT_HPP
