//
// Created by dgmuller on 6/9/25.
//

#ifndef INPUT_HPP
#define INPUT_HPP

#include "system.hpp"
#include "ui/manager.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"

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

        void AddMouseListener(MouseInterface* listener);

        void AddKeyListener(KeyboardInterface* listener);

        void RemoveMouseListener(MouseInterface* listener);

        void RemoveKeyListener(KeyboardInterface* listener);

    private:
        sf::RenderWindow& window;
        ui::Manager& uiManager;
        std::vector<KeyboardInterface*> keyListeners;
        std::vector<MouseInterface*> mouseListeners;
    };
}

#endif //INPUT_HPP
