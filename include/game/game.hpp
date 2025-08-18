//
// Created by dgmuller on 6/7/25.
//

#ifndef GAME_HPP
#define GAME_HPP

#include "../engine/engine.hpp"
#include "../engine/logInterface.hpp"

#include "../ui/config.hpp"
#include "../ui/scene.hpp"

#include "world.hpp"
#include "player.hpp"


namespace farcical::game {
    class Game;

    class GameController final : public engine::Entity, public engine::EventHandler, public engine::LogInterface {
    public:
        GameController() = delete;

        explicit GameController(Game& game);

        ~GameController() override = default;

        void HandleEvent(const engine::Event& event) override;

        static constexpr std::string_view GameControllerID{"GameController"};

    private:
        Game& game;
    };

    class Game {
    public:
        enum class Status {
            Uninitialized = -2,
            Error = -1,
            StoppedSuccessfully = 0,
            IsRunning = 1
        };

        Game() = delete;
        Game(Game const&) = delete;
        Game(Game &&) = delete;
        Game& operator=(Game const&) = delete;
        explicit Game(engine::Engine& engine);
        ~Game() = default;

        Status GetStatus() const;

        engine::Engine& GetEngine() const;

        std::optional<engine::Error> Init();

        std::optional<engine::Error> Update();

        std::optional<engine::Error> Stop();

        std::unique_ptr<World> CreateWorld();

        std::unique_ptr<Player> CreatePlayer();
    private:

        Status status;
        engine::Engine& engine;
        std::unique_ptr<GameController> controller;
    };
}

#endif //GAME_HPP
