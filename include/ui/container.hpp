//
// Created by dgmuller on 6/24/25.
//

#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "widget.hpp"

namespace farcical::ui {
    class Container : public Widget {
    public:
        Container() = delete;

        Container(const Container&) = delete;

        Container(Container&) = delete;

        Container(Container&&) = delete;

        Container& operator=(const Container&) = delete;

        explicit Container(std::string_view name, Type type, Widget* parent = nullptr,
                           bool receiveFocus = false): Widget(name, type, parent, receiveFocus) {
        }

        virtual ~Container() override = default;

        [[nodiscard]] bool IsContainer() const noexcept override { return true; }
        [[nodiscard]] unsigned int GetNumChildren() const { return static_cast<unsigned int>(children.size()); }
        [[nodiscard]] Widget* GetChild(unsigned int index) const { return children[index].get(); }
        void AddChild(std::unique_ptr<Widget> child) { children.push_back(std::move(child)); }

        void Draw(sf::RenderTarget& target) const override {
            for(const auto& child: children) {
                child->Draw(target);
            }
        }

        virtual void DoAction(Action action) override = 0;

    protected:
        std::vector<std::unique_ptr<Widget> > children;
    };
}

#endif //CONTAINER_HPP
