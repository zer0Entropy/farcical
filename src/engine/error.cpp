//
// Created by dgmuller on 9/7/25.
//

#include "../../include/engine/error.hpp"

const std::string farcical::engine::Error::ContextPrefix{"(Context: "};
const std::string farcical::engine::Error::ErrorPrefix{"Error: "};
const std::string farcical::engine::Error::NullPtrMessage{ErrorPrefix + "A null pointer deference was attempted."};
const std::string farcical::engine::Error::InvalidConfigMessage{ErrorPrefix + "Invalid configuration -- "};
const std::string farcical::engine::Error::ResourceNotFoundMessage{ErrorPrefix + "Resource not found -- "};
const std::string farcical::engine::Error::UnexpectedValueMessage{ErrorPrefix + "Unexpected value -- "};
const std::string farcical::engine::Error::DivideByZeroMessage{ErrorPrefix + "Division by zero was attempted."};
const std::string farcical::engine::Error::InvalidPathMessage{ErrorPrefix + "Invalid path -- failed to load from "};
const std::string farcical::engine::Error::WriteFailureMessage{
    ErrorPrefix + "Write failure while attempting to write to path "
};

farcical::engine::Error farcical::engine::GenerateError(Error::Signal signal, std::vector<std::any> args) {
    Error error{signal, "", ""};
    switch(signal) {
        case Error::Signal::None: {
            return error;
        }
        break;
        case Error::Signal::NullPtr: {
            error.message = Error::NullPtrMessage;
        }
        break;
        case Error::Signal::InvalidConfiguration: {
            error.message = Error::InvalidConfigMessage;
            error.message += std::any_cast<std::string>(args.at(0));
            error.message += " = ";
            error.message += std::any_cast<std::string>(args.at(1));
            error.message += ".";
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
        case Error::Signal::DivideByZero: {
            error.message = Error::DivideByZeroMessage;
        }
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
    const int contextIndex = args.size() - 1;
    error.context = std::any_cast<std::string>(args.at(contextIndex));
    return error;
}
