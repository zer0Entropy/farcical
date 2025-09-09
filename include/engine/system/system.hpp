//
// Created by dgmuller on 5/24/25.
//

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "../error.hpp"
#include "../logInterface.hpp"

namespace farcical::engine {
    class System : public LogInterface {
    public:
        enum class ID {
            LogSystem,
            RenderSystem,
            InputSystem,
            EventSystem,
            MusicSystem
        };

        System() = delete;

        explicit System(const System::ID id, LogSystem& logSystem, ErrorGenerator* errorGenerator):
            LogInterface(logSystem),
            id{id},
            errorGenerator{errorGenerator} {
        }

        ~System() override = default;

        void UpdateErrorGenerator(ErrorGenerator* errorGenerator) {
            this->errorGenerator = errorGenerator;
        }

        [[nodiscard]] System::ID GetID() const { return id; }

        virtual void Init() = 0;

        virtual void Update() = 0;

        virtual void Stop() = 0;

    protected:
        System::ID id;
        ErrorGenerator* errorGenerator;
    };
}

#endif //SYSTEM_HPP