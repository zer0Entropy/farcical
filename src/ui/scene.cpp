//
// Created by dgmuller on 6/24/25.
//
#include "../../include/ui/scene.hpp"

farcical::ui::Scene::Scene(std::string_view name, Widget* parent): Container(name, Widget::Type::Scene, parent) {
}

void farcical::ui::Scene::DoAction(Action action) {
}
