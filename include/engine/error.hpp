//
// Created by dgmuller on 6/3/25.
//

#ifndef ERROR_HPP
#define ERROR_HPP

#include <any>
#include <string>
#include <vector>

namespace farcical::engine {
    struct Error {
        enum class Signal {
            None,
            NullPtr,
            InvalidConfiguration,
            ResourceNotFound,
            UnexpectedValue,
            DivideByZero,
            InvalidPath,
            WriteFailure
        };

        Signal      signal;
        std::string message;
        std::string context;

        // These strings are defined in error.cpp
        static const std::string ContextPrefix;
        static const std::string ErrorPrefix;
        static const std::string NullPtrMessage;
        static const std::string InvalidConfigMessage;
        static const std::string ResourceNotFoundMessage;
        static const std::string UnexpectedValueMessage;
        static const std::string DivideByZeroMessage;
        static const std::string InvalidPathMessage;
        static const std::string WriteFailureMessage;
    };

    Error GenerateError(Error::Signal signal, std::vector<std::any> args);
}

#endif //ERROR_HPP
