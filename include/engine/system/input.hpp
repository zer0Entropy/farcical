//
// Created by dgmuller on 6/9/25.
//

#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include <expected>
#include <unordered_map>
#include <SFML/Graphics/RenderWindow.hpp>

#include "log.hpp"
#include "system.hpp"
#include "../component/input.hpp"
#include "../error.hpp"

namespace farcical::engine {
    class InputSystem final : public System {
    public:
        InputSystem() = delete;

        InputSystem(InputSystem&) = delete;

        InputSystem(const InputSystem&) = delete;

        explicit InputSystem(sf::RenderWindow& window, LogSystem& logSystem);

        ~InputSystem() override = default;

        void Init() override;

        void Update() override;

        void Stop() override;

        std::expected<InputComponent*, Error> CreateInputComponent(
            MouseInterface* mouse, KeyboardInterface* keyboard, EntityID parentID);

        std::optional<Error> DestroyInputComponent(EntityID parentID);

    private:
        sf::RenderWindow& window;
        std::unordered_map<EntityID, std::unique_ptr<InputComponent> > components;
        std::vector<MouseInterface*> mouseListeners;
        std::vector<KeyboardInterface*> keyboardListeners;
    };
}

#endif //INPUT_SYSTEM_HPP