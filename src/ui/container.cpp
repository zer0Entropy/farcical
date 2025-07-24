//
// Created by dgmuller on 7/23/25.
//

#include "../../include/ui/container.hpp"

farcical::ui::RootContainer::RootContainer(engine::EntityID id): Container(
    id, ui::Widget::Type::RootContainer, nullptr) {
}

void farcical::ui::RootContainer::DoAction(Action action) {
}
