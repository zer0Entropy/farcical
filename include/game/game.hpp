//
// Created by dgmuller on 6/7/25.
//

#ifndef GAME_HPP
#define GAME_HPP

#include "../engine.hpp"
#include "../ui/scene.hpp"

namespace farcical::game {
    class Game {
    public:
        struct State {
            enum class ID {
                MainMenu,
                StartNewGame,
                LoadSavedGame,
                OptionsMenu
            };
            ID  id;
        };

        Game() = delete;
        Game(Game const&) = delete;
        Game(Game &&) = delete;
        Game& operator=(Game const&) = delete;

        explicit Game(Engine& engine);

        ~Game() = default;

        std::optional<Error> Init();

        std::optional<Error> Update();

        void TransitionToState(State::ID stateID);

        ui::Scene*  LoadScene(std::string_view path);

    private:
        Engine& engine;
        State currentState;
        ui::Scene* currentScene;

        static constexpr std::string_view mainMenuID = "mainMenu";
        static constexpr std::string_view newGameText = "New Game";
        static constexpr std::string_view loadGameText = "Load Game";
        static constexpr std::string_view optionsText = "Options";
        static constexpr std::string_view quitGameText = "Quit Game";

        static constexpr std::string_view mainMenuPath = "dat/mainMenu.json";
    };
}

#endif //GAME_HPP
