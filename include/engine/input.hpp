//
// Created by dgmuller on 6/9/25.
//

#ifndef INPUT_HPP
#define INPUT_HPP

#include <SFML/Graphics/RenderWindow.hpp>

#include "log.hpp"
#include "system.hpp"
#include "../keyboard.hpp"
#include "../mouse.hpp"

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

        void AddMouseListener(MouseInterface* listener);

        void AddKeyListener(KeyboardInterface* listener);

        void RemoveMouseListener(MouseInterface* listener);

        void RemoveKeyListener(KeyboardInterface* listener);

    private:
        sf::RenderWindow& window;
        std::vector<KeyboardInterface*> keyListeners;
        std::vector<MouseInterface*> mouseListeners;
    };
}

#endif //INPUT_HPP
