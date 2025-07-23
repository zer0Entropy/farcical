//
// Created by dgmuller on 7/21/25.
//
#include "../../include/engine/logInterface.hpp"
#include "../../include/engine/log.hpp"

farcical::engine::LogInterface::LogInterface(LogSystem& logSystem): logSystem{logSystem} {
}

void farcical::engine::LogInterface::WriteToLog(std::string_view message) const {
    logSystem.AddMessage(message);
}
