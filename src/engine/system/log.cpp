//
// Created by dgmuller on 7/21/25.
//
#include "../../../include/engine/system/log.hpp"
#include "../../../include/resource/manager.hpp"

farcical::engine::LogSystem::LogSystem(ResourceID logID,
                                       std::string_view logPath) : System(System::ID::LogSystem, *this),
                                                                   logParams{logID, logPath},
                                                                   resourceManager{nullptr},
                                                                   log{nullptr} {
}

void farcical::engine::LogSystem::Init() {
    if(!resourceManager) {
        return;
    }
    const auto& createHandle{
        resourceManager->CreateResourceHandle(logParams.first, ResourceHandle::Type::Log, logParams.second)
    };
    if(createHandle.has_value()) {
        ResourceHandle* handle{createHandle.value()};
        const auto& loadLogFile{resourceManager->GetLog(handle->id)};
        if(loadLogFile.has_value()) {
            log = loadLogFile.value();
            AddMessage("LogSystem initialized.");
            resourceManager->WriteLog(handle->id, buffer);
            buffer.clear();
        } // if requestLog == success
    } // if createHandle == success
}

void farcical::engine::LogSystem::Update() {
    resourceManager->AppendToLog(log, buffer);
    buffer.clear();
}

void farcical::engine::LogSystem::Stop() {
    AddMessage("LogSystem shutting down...");
    if(!buffer.empty()) {
        Update();
    }
    resourceManager->DestroyResourceHandle(logParams.first, ResourceHandle::Type::Log);
}

void farcical::engine::LogSystem::AddResourceManager(ResourceManager* resourceManager) {
    this->resourceManager = resourceManager;
}

void farcical::engine::LogSystem::AddMessage(std::string_view message) {
    buffer.emplace_back(message);
}