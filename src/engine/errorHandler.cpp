//
// Created by dgmuller on 9/7/25.
//

#include "../../include/engine/errorHandler.hpp"

#include "../../include/engine/system/log.hpp"

farcical::engine::ErrorHandler::ErrorHandler() : ErrorGenerator(),
                                                 eventSystem{nullptr},
                                                 logSystem{nullptr},
                                                 errorOccurred{false} { lastError.signal = Error::Signal::None; }

void farcical::engine::ErrorHandler::SetEventSystemPtr(engine::EventSystem* eventSystem) {
    this->eventSystem = eventSystem;
}

void farcical::engine::ErrorHandler::SetLogSystemPtr(engine::LogSystem* logSystem) { this->logSystem = logSystem; }

farcical::engine::Error
farcical::engine::ErrorHandler::GenerateError(Error::Signal signal, std::vector<std::any> args) {
    Error error{signal, "", ""};
    switch(signal) {
        case Error::Signal::None: { return error; }
        break;
        case Error::Signal::NullPtr: { error.message = Error::NullPtrMessage; }
        break;
        case Error::Signal::InvalidConfiguration: {
            error.message = Error::InvalidConfigMessage;
            const int numMessages{static_cast<int>(args.size()) - 1};
            for(int index = 0; index < numMessages; ++index) {
                error.message += std::any_cast<std::string>(args[index]);
            } // for each arg intended to supplement the error message
        }
        break;
        case Error::Signal::ResourceNotFound: {
            error.message = Error::ResourceNotFoundMessage;
            error.message += std::any_cast<std::string>(args.at(0));
        }
        break;
        case Error::Signal::UnexpectedValue: {
            error.message = Error::UnexpectedValueMessage;
            error.message += std::any_cast<std::string>(args.at(0));
            error.message += " = ";
            error.message += std::any_cast<std::string>(args.at(1));
            error.message += ".";
        }
        break;
        case Error::Signal::DivideByZero: { error.message = Error::DivideByZeroMessage; }
        break;
        case Error::Signal::InvalidPath: {
            error.message = Error::InvalidPathMessage;
            error.message += std::any_cast<std::string>(args.at(0));
            error.message += ".";
        }
        break;
        case Error::Signal::WriteFailure: {
            error.message = Error::WriteFailureMessage;
            error.message += std::any_cast<std::string>(args.at(0));
            error.message += ".";
        }
        break;
    } // switch
    const int contextIndex = static_cast<int>(args.size()) - 1;
    error.context = std::any_cast<std::string>(args.at(contextIndex));

    errorOccurred = true;
    lastError = error;
    const std::string logMsg{"ErrorSignal #" + std::to_string(static_cast<int>(error.signal))};
    logSystem->WriteToLog(error.message);
    logSystem->WriteToLog(error.context);

    return error;
}

bool farcical::engine::ErrorHandler::HasErrorOccurred() const { return errorOccurred; }
const farcical::engine::Error& farcical::engine::ErrorHandler::GetError() const { return lastError; }
