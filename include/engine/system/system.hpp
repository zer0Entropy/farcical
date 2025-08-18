//
// Created by dgmuller on 5/24/25.
//

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "../logInterface.hpp"

namespace farcical::engine {
    class System : public LogInterface {
    public:

        enum class ID {
            LogSystem,
            RenderSystem,
            InputSystem,
            EventSystem
        };

        System() = delete;

        explicit System(const System::ID id, LogSystem& logSystem): LogInterface(logSystem),
                                                                    id{id} {
        }

        virtual ~System() = default;

        [[nodiscard]] System::ID GetID() const { return id; }

        virtual void Init() = 0;

        virtual void Update() = 0;

        virtual void Stop() = 0;

    protected:
        System::ID id;
    };
}

#endif //SYSTEM_HPP
