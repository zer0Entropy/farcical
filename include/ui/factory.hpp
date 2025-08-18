//
// Created by dgmuller on 7/24/25.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "config.hpp"
#include "decoration.hpp"
#include "menu.hpp"
#include "scene.hpp"
#include "../engine/error.hpp"
#include "../engine/system/render.hpp"
#include "../engine/system/input.hpp"

namespace farcical::ui::factory {
    [[nodiscard]] std::expected<Decoration*, engine::Error> CreateDecoration(
        engine::RenderSystem& renderSystem, Scene* scene, const DecorationProperties& properties);

    std::optional<engine::Error> DestroyDecoration(engine::RenderSystem& renderSystem, Scene* scene,
                                                   Decoration* decoration);

    [[nodiscard]] std::expected<Text*, engine::Error> CreateText(
        engine::RenderSystem& renderSystem, Scene* scene, const WidgetProperties& properties,
        sf::FloatRect bounds = sf::FloatRect{{0.0f, 0.0f}, {0.0f, 0.0f}});

    std::optional<engine::Error> DestroyText(engine::RenderSystem& renderSystem, Scene* scene, Text* text);

    [[nodiscard]] std::expected<Menu*, engine::Error> CreateMenu(
        engine::RenderSystem& renderSystem,
        engine::InputSystem& inputSystem,
        engine::EventSystem& eventSystem,
        Scene* scene,
        const MenuProperties& properties);

    std::optional<engine::Error> DestroyMenu(
        engine::RenderSystem& renderSystem,
        engine::InputSystem& inputSystem,
        engine::EventSystem& eventSystem,
        Scene* scene,
        Menu* menu);

    [[nodiscard]] std::expected<Button*, engine::Error> CreateButton(
        engine::RenderSystem& renderSystem,
        Scene* scene,
        Menu* menu,
        const MenuProperties& menuProperties,
        const ButtonProperties& buttonProperties);

    [[nodiscard]] std::expected<RadioButton*, engine::Error> CreateRadioButton(
        engine::RenderSystem& renderSystem,
        Scene* scene,
        Menu* menu,
        const MenuProperties& menuProperties,
        const WidgetProperties& radioButtonProperties);

    [[nodiscard]] std::vector<sf::Texture*> GetButtonTextures(const Scene& scene, const MenuProperties& properties);

    [[nodiscard]] std::vector<sf::Texture*> GetRadioButtonTextures(const Scene& scene, const MenuProperties& properties);

}
#endif //FACTORY_HPP
