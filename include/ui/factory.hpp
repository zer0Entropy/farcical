//
// Created by dgmuller on 7/24/25.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "config.hpp"
#include "container.hpp"
#include "decoration.hpp"
#include "menu.hpp"
#include "scene.hpp"
#include "../engine/error.hpp"

namespace farcical::game {
    class Game;
}

namespace farcical::ui::factory {
    [[nodiscard]] std::expected<Decoration*, engine::Error> CreateDecoration(
        const game::Game& game, const DecorationConfig& config, Container* parent);

    std::optional<engine::Error> DestroyDecoration(const game::Game& game, Decoration* decoration);

    [[nodiscard]] std::expected<Text*, engine::Error> CreateText(
        const game::Game& game, const TextConfig& config, Container* parent);

    std::optional<engine::Error> DestroyText(const game::Game& game, Text* text);

    [[nodiscard]] std::expected<Menu*, engine::Error> CreateMenu(
        const game::Game& game, const MenuConfig& config, Container* parent);

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

    [[nodiscard]] std::expected<Text*, engine::Error> CreateMenuLabel(
        const game::Game& game,
        const MenuItemConfig& itemConfig,
        const MenuConfig& menuConfig,
        MenuItem* item);

    [[nodiscard]] MenuItemCollection CreateMenuItemCollection(const MenuConfig& config);

    [[nodiscard]] std::vector<sf::Texture*> GetButtonTextures(const Scene& scene, const ButtonConfig& config);
}

#endif //FACTORY_HPP
