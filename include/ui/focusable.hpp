//
// Created by dgmuller on 9/3/25.
//

#ifndef FOCUSABLE_HPP
#define FOCUSABLE_HPP

namespace farcical::ui {
    class Focusable {
    public:
        Focusable() : focused{false} {
        }

        virtual ~Focusable() = default;

        bool HasFocus() const { return focused; }

        virtual void OnReceiveFocus() = 0;

        virtual void OnLoseFocus() = 0;

    protected:
        bool focused;
    };
}

#endif // FOCUSABLE_HPP
