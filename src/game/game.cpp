//
// Created by dgmuller on 6/7/25.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../../include/game/game.hpp"
#include "../../include/ui/menu.hpp"
#include "../../include/ui/decoration.hpp"
#include "../../include/engine/system/log.hpp"
#include "../../include/ui/factory.hpp"

farcical::game::GameController::GameController(Game& game) : Entity(engine::EntityID{GameControllerID}),
                                                             EventHandler(),
                                                             LogInterface(game.GetEngine().GetLogSystem()), game{game} {
}

void farcical::game::GameController::HandleEvent(const engine::Event& event) {
    ui::SceneManager& sceneManager{game.GetEngine().GetSceneManager()};
    ResourceManager& resourceManager{game.GetEngine().GetResourceManager()};
    ui::Scene* currentScene{sceneManager.GetCurrentScene()};

    if(event.type == engine::Event::Type::NotifyErrorOccurred) {
        WriteToLog("GameController received 'NotifyErrorOccurred' event.");
        game.Stop();
    } else if(event.type == engine::Event::Type::QuitGame) {
        WriteToLog("GameController received 'QuitGame' event.");
        game.Stop();
    } // if event.type == QuitGame

    else if(event.type == engine::Event::Type::ApplyEngineConfig) {
        WriteToLog("GameController received 'ApplyEngineConfig' event.");
        //const auto& createConfig{game.GetEngine().CreateConfig()};
        //engine::Config config{createConfig.value()};
        engine::Config currentConfig{game.GetEngine().GetConfig()};
        engine::EntityID displayOptionsID{"displayOptions"};
        ui::Widget* displayOptionsWidget{
            game.GetEngine().GetSceneManager().GetCurrentScene()->FindChild(displayOptionsID)
        };
        if(displayOptionsWidget) {
            ui::Menu* menu{dynamic_cast<ui::Menu*>(displayOptionsWidget)};
            ui::Widget* focusedWidget{currentScene->GetFocusedWidget()};
            if(focusedWidget) {
                engine::EntityID focusedWidgetID{focusedWidget->GetID()};
                const std::string radioButtonString{"RadioButton"};
                engine::EntityID labelID{
                    focusedWidgetID.substr(0, focusedWidgetID.length() - radioButtonString.length())
                };
                labelID += "Label";
                ui::Text* radioButtonLabel{dynamic_cast<ui::Text*>(menu->FindChild(labelID))};
                if(radioButtonLabel) {
                    const std::string contents{radioButtonLabel->GetContents()};
                    const auto& getWindowProperties{GetWindowProperties(contents)};
                    bool changed{false};
                    if(getWindowProperties.has_value()) {
                        const WindowProperties& windowProperties{getWindowProperties.value()};
                        if(currentConfig.windowProperties != windowProperties) {
                            currentConfig.windowProperties = windowProperties;
                            changed = true;
                        } // if displayMode changed
                    } // if getWindowProperties
                    if(changed) {
                        const auto& applyConfig{game.GetEngine().ApplyConfig(currentConfig)};
                    }
                } // if radioButtonLabel
            } // if selectedWidget
        } // if displayOptionsWidget
    } // else if event.type == ApplyEngineConfig

    else if(event.type == engine::Event::Type::CreateScene) {
        std::string logMsg{"GameController received 'CreateScene' event (args=\""};
        for(const auto& arg: event.args) {
            if(arg.type() == typeid(std::string)) {
                const std::string argString{std::any_cast<std::string>(arg)};
                logMsg.append(argString);
            } // string
        } // for each arg
        logMsg.append("\").");
        WriteToLog(logMsg);
        const engine::EntityID nextSceneID{std::any_cast<std::string>(event.args.at(0))};

        const auto& destroyCurrentScene{sceneManager.DestroyCurrentScene()};
        if(!destroyCurrentScene.has_value()) {
            const auto& createNextScene{sceneManager.SetCurrentScene(nextSceneID)};
            if(!createNextScene.has_value()) {
                game.Stop();
            } // if createNextScene == failure
        } // if destroyCurrentScene == success
    } // else if event.type == CreateScene
}

farcical::game::Game::Game(engine::Engine& engine):
    status{Status::Uninitialized},
    engine{engine},
    controller{nullptr} {
}

farcical::game::Game::Status farcical::game::Game::GetStatus() const {
    return status;
}

farcical::engine::Engine& farcical::game::Game::GetEngine() const {
    return engine;
}

std::optional<farcical::engine::Error> farcical::game::Game::Init() {
    engine::LogSystem& logSystem{engine.GetLogSystem()};
    if(status == Status::IsRunning) {
        logSystem.AddMessage("Error: Game.Init() called, but Game is already initialized!");
    } else if(status == Status::Error) {
        logSystem.AddMessage("Unknown error occurred in game.Init() [game.status == Error]");
    }
    if(status == Status::IsRunning || status == Status::Error) {
        return std::nullopt;
    } // if status == IsRunning || Error
    logSystem.AddMessage("Initializing game...");


    logSystem.AddMessage("Creating GameController...");
    controller = std::make_unique<GameController>(*this);

    // Subscribe GameController to QuitGame, CreateScene, DestroyScene Events
    logSystem.AddMessage("Registering EventHandlers...");
    engine::EventSystem& eventSystem{engine.GetEventSystem()};
    const std::vector<engine::Event::Type> handledTypes{
        engine::Event::Type::NotifyErrorOccurred,
        engine::Event::Type::QuitGame,
        engine::Event::Type::ApplyEngineConfig,
        engine::Event::Type::CreateScene
    };
    const auto& createEventComponent{
        eventSystem.CreateEventComponent(handledTypes, controller.get(), controller->GetID())
    };
    if(!createEventComponent.has_value()) {
        const std::string failMsg{"Error: Failed to create EventComponent for " + controller->GetID() + "."};
        return engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg};
    } // if createEventComponent == failure

    status = Status::IsRunning;

    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::Update() {
    return std::nullopt;
}

std::optional<farcical::engine::Error> farcical::game::Game::Stop() {
    if(status == Status::IsRunning) {
        if(controller) {
            controller.reset();
        }
        status = Status::StoppedSuccessfully;
    }
    return std::nullopt;
}

std::unique_ptr<farcical::game::World> farcical::game::Game::CreateWorld() {
    return std::make_unique<World>();
}

std::unique_ptr<farcical::game::Player> farcical::game::Game::CreatePlayer() {
    return std::make_unique<Player>();
}