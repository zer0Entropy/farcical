//
// Created by dgmuller on 6/22/25.
//
#include "../../include/engine/event.hpp"
#include "../../include/game/game.hpp"

farcical::engine::EventSystem::EventSystem(game::Game& game, Engine& engine): System(ID::EventSystem,
                                                                                  engine.GetLogSystem()),
                                                                              game{game}, engine{engine} {
}

void farcical::engine::EventSystem::RegisterHandler(Event::Type type, EventHandler* handler) {
    handlers.insert(std::make_pair(type, handler));
    return;
}

void farcical::engine::EventSystem::UnregisterHandler(Event::Type type, EventHandler* handler) {
    const auto& findHandler{handlers.find(type)};
    if(findHandler != handlers.end()) {
        handlers.erase(findHandler);
    } // if handler found
    return;
}

void farcical::engine::EventSystem::Enqueue(const Event& event) {
    eventQueue.push_back(event);
    return;
}

void farcical::engine::EventSystem::Init() {
    WriteToLog("EventSystem initialized.");
    return;
}

void farcical::engine::EventSystem::Update() {
    // Iterate through a copy of the eventQueue, to prevent changes to the queue from messing up our iteration
    const auto queueCopy{eventQueue};
    eventQueue.clear();

    for(const auto& event: queueCopy) {
        const auto& findHandler{handlers.find(event.type)};
        if(findHandler != handlers.end()) {
            findHandler->second->HandleEvent(event);
        } // if handler found
    } // for each Event in queueCopy
    return;
}

void farcical::engine::EventSystem::Stop() {
    eventQueue.clear();
    WriteToLog("EventSystem successfully shut down.");
    return;
}
