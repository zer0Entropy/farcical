//
// Created by dgmuller on 8/19/25.
//
#include "../../../include/engine/system/music.hpp"

farcical::engine::MusicSystem::MusicSystem(ResourceManager& resourceManager,
                                           LogSystem& logSystem) : System(System::ID::MusicSystem, logSystem),
                                                                   resourceManager{resourceManager},
                                                                   currentMusicHandle{nullptr},
                                                                   currentMusic{nullptr} {
}

void farcical::engine::MusicSystem::PlayMusic() {
    if(currentMusic) {
        currentMusic->play();
    } // if currentMusic
}

void farcical::engine::MusicSystem::PauseMusic() {
    if(currentMusic) {
        currentMusic->pause();
    } // if currentMusic
}

void farcical::engine::MusicSystem::StopMusic() {
    if(currentMusic) {
        currentMusic->stop();
    } // if currentMusic
}

farcical::ResourceID farcical::engine::MusicSystem::GetCurrentMusic() const {
    if(currentMusicHandle) {
        return currentMusicHandle->id;
    } // if currentMusicHandle
    return ResourceID{};
}

void farcical::engine::MusicSystem::SetCurrentMusic(ResourceID musicID) {
    currentMusicHandle = resourceManager.GetResourceHandle(musicID);
    if(!currentMusicHandle) {
        return;
    } // if !handle
    const auto& getMusic{resourceManager.GetMusic(musicID)};
    if(getMusic.has_value()) {
        currentMusic = getMusic.value();
    } // if getMusic == success
}

void farcical::engine::MusicSystem::ClearCurrentMusic() {
    if(currentMusic) {
        currentMusic->stop();
    } // if currentMusic
    currentMusicHandle = nullptr;
    currentMusic = nullptr;
}

void farcical::engine::MusicSystem::Init() {
}

void farcical::engine::MusicSystem::Update() {
}

void farcical::engine::MusicSystem::Stop() {
}
