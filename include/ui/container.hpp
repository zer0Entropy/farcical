//
// Created by dgmuller on 6/24/25.
//

#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <memory>
#include <vector>
#include "widget.hpp"

namespace farcical::ui {
    class Container : public Widget {
    public:
        Container() = delete;
        Container(const Container&) = delete;
        Container(Container&) = delete;
        Container(Container&&) = delete;
        Container& operator=(const Container&) = delete;
        explicit Container( engine::EntityID id,
                            Type type,
                            Container* parent = nullptr,
                            bool receiveFocus = false):
            Widget(id, type, parent, receiveFocus) {
        }
        virtual ~Container() override = default;

        [[nodiscard]] bool IsContainer() const noexcept override { return true; }
        [[nodiscard]] unsigned int GetNumChildren() const { return static_cast<unsigned int>(children.size()); }
        [[nodiscard]] Widget* GetChild(unsigned int index) const { return children[index].get(); }
        [[nodiscard]] std::vector<Widget*> GetChildren() const {
            std::vector<Widget*> childList;
            for(const auto& child: children) {
                childList.push_back(child.get());
            }
            return childList;
        }

        [[nodiscard]] Widget* FindChild(engine::EntityID childID) const {
            Widget* childPtr{nullptr};
            for(const auto& child: children) {
                if(child->GetID() == childID) {
                    childPtr = child.get();
                } // if IDs match
                else if(child->IsContainer()) {
                    Container* container = dynamic_cast<Container*>(child.get());
                    childPtr = container->FindChild(childID);
                }
                if(childPtr) {
                    break;
                }
            } // for each child in children
            return childPtr;
        }

        virtual void AddChild(std::unique_ptr<Widget> child) { children.push_back(std::move(child)); }

        virtual void RemoveChild(engine::EntityID childID) {
            for(auto it = children.begin(); it != children.end(); ++it) {
                if((*it)->GetID() == childID) {
                    children.erase(it);
                    return;
                } // if IDs match
            } // for each child in children
        }

        virtual void DoAction(Action action) override = 0;

    protected:
        std::vector<std::unique_ptr<Widget> > children;
    };

}

#endif //CONTAINER_HPP
