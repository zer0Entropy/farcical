//
// Created by dgmuller on 7/24/25.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "config.hpp"
#include "container.hpp"
#include "menu.hpp"
#include "scene.hpp"
#include "../engine/error.hpp"

namespace farcical::game {
    class Game;
}

namespace farcical::ui::factory {

    [[nodiscard]] std::expected<Label*, engine::Error> CreateTitle(const game::Game& game, const LabelConfig& config, Container* parent);

    std::optional<engine::Error> DestroyTitle(const game::Game& game, Label* title);

    [[nodiscard]] std::expected<Menu*, engine::Error> CreateMenu(const game::Game& game, const MenuConfig& config, Container* parent);

    std::optional<engine::Error> DestroyMenu(const game::Game& game, Menu* menu);

    [[nodiscard]] std::expected<MenuItem*, engine::Error> CreateMenuItem(
        const game::Game& game,
        const MenuConfig& menuConfig,
        const MenuItemConfig& config,
        Menu* menu);

    std::optional<engine::Error> DestroyMenuItem(const game::Game& game, MenuItem* item);

    [[nodiscard]] std::expected<Button*, engine::Error> CreateMenuButton(
        const game::Game& game,
        const MenuItemConfig& itemConfig,
        const MenuConfig& menuConfig,
        MenuItem* item);

    [[nodiscard]] std::expected<Label*, engine::Error> CreateMenuLabel(
        const game::Game& game,
        const MenuItemConfig& itemConfig,
        const MenuConfig& menuConfig,
        MenuItem* item);

    [[nodiscard]] MenuItemCollection CreateMenuItemCollection(const MenuConfig& config);

    [[nodiscard]] std::vector<sf::Texture*> GetButtonTextures(const Scene& scene, const ButtonConfig& config);

}

#endif //FACTORY_HPP
