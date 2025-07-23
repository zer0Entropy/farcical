//
// Created by dgmuller on 7/21/25.
//

#ifndef LOG_INTERFACE_HPP
#define LOG_INTERFACE_HPP

#include <string>

namespace farcical::engine {
    class LogSystem;

    class LogInterface {
    public:
        LogInterface() = delete;

        explicit LogInterface(LogSystem& logSystem);

        virtual ~LogInterface() = default;

        void WriteToLog(std::string_view message) const;

    private:
        LogSystem& logSystem;
    };
}

#endif //LOG_INTERFACE_HPP
