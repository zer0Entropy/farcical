//
// Created by dgmuller on 9/7/25.
//

#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "error.hpp"
#include "logInterface.hpp"
#include "../../include/engine/system/event.hpp"

namespace farcical::engine {
    class ErrorHandler final : public ErrorGenerator {
    public:
        ErrorHandler();

        ~ErrorHandler() override = default;

        void SetEventSystemPtr(engine::EventSystem* eventSystem);

        void SetLogSystemPtr(engine::LogSystem* logSystem);

        [[nodiscard]] Error GenerateError(Error::Signal signal, std::vector<std::any> args) override;

        [[nodiscard]] bool HasErrorOccurred() const;

        [[nodiscard]] const Error& GetError() const;

    private:
        engine::EventSystem* eventSystem;
        engine::LogSystem* logSystem;
        bool errorOccurred;
        Error lastError;
    };
}

#endif //ERROR_HANDLER_HPP
