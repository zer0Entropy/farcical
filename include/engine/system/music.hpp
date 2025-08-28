//
// Created by dgmuller on 8/19/25.
//

#ifndef MUSIC_SYSTEM_HPP
#define MUSIC_SYSTEM_HPP

#include <SFML/Audio/Music.hpp>
#include "system.hpp"
#include "../logInterface.hpp"
#include "../../resource/manager.hpp"

namespace farcical::engine {
    class MusicSystem final : public System {
    public:
        MusicSystem() = delete;

        explicit MusicSystem(ResourceManager& resourceManager, LogSystem& logSystem);

        ~MusicSystem() override = default;

        void PlayMusic();

        void PauseMusic();

        void StopMusic();

        void SetLoop(bool loop);

        ResourceID GetCurrentMusic() const;

        void SetCurrentMusic(ResourceID musicID);

        void ClearCurrentMusic();

        void Init() override;

        void Update() override;

        void Stop() override;

    private:
        ResourceManager& resourceManager;
        ResourceHandle* currentMusicHandle;
        sf::Music* currentMusic;
    };
}

#endif // MUSIC_SYSTEM_HPP
