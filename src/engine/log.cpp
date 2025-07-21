//
// Created by dgmuller on 7/21/25.
//
#include "../../include/engine/log.hpp"
#include "../../include/resource/manager.hpp"

farcical::engine::LogSystem::LogSystem(ResourceID logID, std::string_view logPath, ResourceManager& resourceManager):
    System(System::ID::LogSystem, *this),
    logParams{logID, logPath},
    resourceManager{resourceManager},
    log{nullptr} {
}

void farcical::engine::LogSystem::Init() {
    const auto& createHandle{
        resourceManager.CreateResourceHandle(logParams.first, ResourceHandle::Type::Log, logParams.second)
    };
    if(createHandle.has_value()) {
        ResourceHandle* handle{createHandle.value()};
        const auto& loadLogFile{resourceManager.GetLog(handle->id)};
        if(loadLogFile.has_value()) {
            log = loadLogFile.value();
        } // if requestLog == success
    } // if createHandle == success
}

void farcical::engine::LogSystem::Update() {
    for(const auto& message: buffer) {
        resourceManager.AppendToLog(logParams.first, message);
    } // for each message in buffer
}

void farcical::engine::LogSystem::Stop() {
    if(!buffer.empty()) {
        Update();
    }
    resourceManager.DestroyResourceHandle(logParams.first, ResourceHandle::Type::Log);
}

void farcical::engine::LogSystem::AddMessage(std::string_view message) {
    buffer.emplace_back(message);
}
