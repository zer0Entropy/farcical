//
// Created by dgmuller on 7/24/25.
//

#ifndef ROOT_HPP
#define ROOT_HPP

#include <unordered_map>
#include "container.hpp"
#include "menu.hpp"
#include "../engine/input.hpp"

namespace farcical::ui {

    class RootContainer final : public Container {
    public:
        RootContainer() = delete;
        RootContainer(const RootContainer&) = delete;
        RootContainer(RootContainer&) = delete;
        RootContainer(RootContainer&&) = delete;
        RootContainer& operator=(const RootContainer&) = delete;
        RootContainer& operator=(const RootContainer&&) = delete;
        explicit RootContainer(engine::EntityID id, game::Game& game);
        ~RootContainer() override = default;

        void DoAction(Action action) override;

        std::expected<MenuController*, engine::Error> CreateMenuController(
            Menu* menu, engine::EventSystem& eventSystem, engine::InputSystem& inputSystem);
        std::optional<engine::Error> DestroyMenuController(Menu* menu, engine::InputSystem& inputSystem);
        MenuController* GetMenuController(engine::EntityID menuID);

    private:
        game::Game& game;
        engine::LogSystem* logSystem;
        std::unordered_map<engine::EntityID, std::unique_ptr<MenuController>> menuControllers;

    };

}

#endif //ROOT_HPP
