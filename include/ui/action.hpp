//
// Created by dgmuller on 6/18/25.
//

#ifndef ACTION_HPP
#define ACTION_HPP

namespace farcical::ui {
    struct Action {
        enum class Type {
            MoveFocusUp,
            MoveFocusDown,
            MoveFocusLeft,
            MoveFocusRight,
            ConfirmSelection,

            ReceiveFocus,
            LoseFocus,

            SetPressedTrue,
            SetPressedFalse
        };

        Type type;
    };

    class ActionHandler {
    public:
        virtual ~ActionHandler() = default;

        virtual void DoAction(Action action) = 0;
    };
}

#endif //ACTION_HPP
