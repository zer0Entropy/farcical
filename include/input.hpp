//
// Created by dgmuller on 6/9/25.
//

#ifndef INPUT_HPP
#define INPUT_HPP

#include "system.hpp"

namespace farcical {
    class InputSystem final: public System {
    public:
        InputSystem();
        ~InputSystem();

        void Init() override;
        void Update() override;
        void Stop() override;
    private:
    };
}

#endif //INPUT_HPP
