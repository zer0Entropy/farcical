//
// Created by dgmuller on 7/21/25.
//

#ifndef LOG_HPP
#define LOG_HPP

#include <vector>
#include "system.hpp"
#include "../resource/resource.hpp"

namespace farcical {
    class ResourceManager;
}

namespace farcical::engine {
    struct Log {
        ResourceParameters resource;
        std::vector<std::string> contents;
    };

    class LogSystem final : public System {
    public:
        LogSystem() = delete;

        LogSystem(ResourceID logID, std::string_view logPath);

        ~LogSystem() override = default;

        void Init() override;

        void Update() override;

        void Stop() override;

        void AddResourceManager(ResourceManager* resourceManager);

        void AddMessage(std::string_view message);

    private:
        ResourceParameters logParams;
        ResourceManager* resourceManager;
        Log* log;
        std::vector<std::string> buffer;
    };
}

#endif //LOG_HPP
