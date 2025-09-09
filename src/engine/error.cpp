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
