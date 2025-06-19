//
// Created by dgmuller on 6/9/25.
//

#ifndef INPUT_HPP
#define INPUT_HPP

#include "system.hpp"
#include "ui/manager.hpp"

namespace farcical {
    class InputSystem final : public System {
    public:
        InputSystem() = delete;
        InputSystem(InputSystem&) = delete;
        InputSystem(const InputSystem&) = delete;
        explicit InputSystem(sf::RenderWindow& window, ui::Manager& manager);
        ~InputSystem() override = default;

        void Init() override;

        void Update() override;

        void Stop() override;

    private:
        sf::RenderWindow&     window;
        ui::Manager&          uiManager;
    };
}

#endif //INPUT_HPP
