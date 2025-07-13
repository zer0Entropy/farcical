//
// Created by dgmuller on 5/24/25.
//

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

namespace farcical::engine
{
    class System
    {
    public:

        enum class ID {
            RenderSystem,
            InputSystem,
            EventSystem
        };

        System() = delete;
        explicit System(const System::ID id): id(id) {}
        virtual ~System() = default;

        [[nodiscard]] System::ID GetID() const;

        virtual void Init() = 0;

        virtual void Update() = 0;

        virtual void Stop() = 0;

    protected:
        System::ID id;
    };
}

#endif //SYSTEM_HPP
