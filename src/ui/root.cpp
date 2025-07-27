//
// Created by dgmuller on 7/23/25.
//

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../../include/ui/root.hpp"
#include "../../include/game/game.hpp"

farcical::ui::RootContainer::RootContainer(engine::EntityID id, game::Game& game):
    Container(id, ui::Widget::Type::RootContainer, nullptr),
    game{game},
    logSystem{&game.GetEngine().GetLogSystem()} {

}

void farcical::ui::RootContainer::DoAction(Action action) {
}

std::expected<farcical::ui::MenuController*, farcical::engine::Error> farcical::ui::RootContainer::CreateMenuController(
    Menu* menu, engine::EventSystem& eventSystem, engine::InputSystem& inputSystem) {
    // Check if this MenuController already exists
    const auto& findController{menuControllers.find(menu->GetID())};
    // If it does, destroy it
    if(findController != menuControllers.end()) {
        menuControllers.erase(findController);
    } // if menuController found

    const auto& createMenuController{
        menuControllers.emplace(menu->GetID(), std::make_unique<MenuController>(menu, eventSystem))
    };
    if(createMenuController.second) {
        MenuController* controller{createMenuController.first->second.get()};
        inputSystem.AddKeyListener(controller);
        inputSystem.AddMouseListener(controller);
        return controller;
    } // if createMenuController == success
    const std::string failMsg{"Error: Failed to create MenuController for Menu (id=" + menu->GetID() + ")."};
    return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
}

std::optional<farcical::engine::Error> farcical::ui::RootContainer::DestroyMenuController(Menu* menu, engine::InputSystem& inputSystem) {
    const auto& findController{menuControllers.find(menu->GetID())};
    if(findController != menuControllers.end()) {
        inputSystem.RemoveKeyListener(findController->second.get());
        inputSystem.RemoveMouseListener(findController->second.get());
        menuControllers.erase(findController);
        return std::nullopt;
    } // if menuController found
    const std::string failMsg{"Error: Attempted to destroy MenuController for menu (id=" + menu->GetID() + "), but no such MenuController was found."};
    return engine::Error{engine::Error::Signal::ResourceNotFound, failMsg};
}

farcical::ui::MenuController* farcical::ui::RootContainer::GetMenuController(engine::EntityID menuID) {
    MenuController* controller{nullptr};
    const auto& findController{menuControllers.find(menuID)};
    if(findController != menuControllers.end()) {
        controller = findController->second.get();
    } // if menuController found
    return controller;
}
