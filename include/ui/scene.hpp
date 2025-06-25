//
// Created by dgmuller on 6/24/25.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include "container.hpp"

namespace farcical::ui {
    class Scene final : public Container {
    public:
        Scene() = delete;
        Scene(const Scene&) = delete;
        Scene(Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(const Scene&) = delete;
        explicit Scene(std::string_view name, Widget* parent);
        ~Scene() override = default;

        void DoAction(Action action) override;
    private:
    };
}

#endif //SCENE_HPP
