//
// Created by dgmuller on 6/3/25.
//

#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

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
    };

}

#endif //ERROR_HPP
