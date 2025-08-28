//
// Created by dgmuller on 6/22/25.
//
#include "../../../include/engine/system/event.hpp"
#include "../../../include/game/game.hpp"

farcical::engine::EventSystem::EventSystem(game::Game& game, Engine& engine) : System(ID::EventSystem,
                                                                                   engine.GetLogSystem()),
                                                                               game{game},
                                                                               engine{engine} {
}

void farcical::engine::EventSystem::Enqueue(const Event& event) {
    eventQueue.push_back(event);
}

void farcical::engine::EventSystem::Init() {
    WriteToLog("EventSystem initialized.");
}

void farcical::engine::EventSystem::Update() {
    // Iterate through a copy of the eventQueue, to prevent changes to the queue from messing up our iteration
    const auto queueCopy{eventQueue};
    eventQueue.clear();

    for(const auto& event: queueCopy) {
        // Ensure EventType is valid
        const int eventIndex{static_cast<int>(event.type)};
        if(eventIndex >= 0 && eventIndex < static_cast<int>(Event::Type::NumEventTypes)) {
            const EventHandlerList& handlerList{GetEventHandlerList(event.type)};
            // Each EventHandler in the list handles the Event in sequence
            for(const auto& handler: handlerList) {
                handler->HandleEvent(event);
            } // for each EventHandler* in HandlerList
        } // if EventType is a valid value
    } // for each Event in queueCopy
}

void farcical::engine::EventSystem::Stop() {
    eventQueue.clear();
    WriteToLog("EventSystem successfully shut down.");
}

std::expected<farcical::engine::EventComponent*, farcical::engine::Error> farcical::engine::EventSystem::
CreateEventComponent(const std::vector<Event::Type>& handledTypes, EventHandler* handler, EntityID parentID) {
    const auto& createComponentResult{
        components.emplace(parentID, std::make_unique<EventComponent>(parentID))
    };
    if(!createComponentResult.second) {
        const std::string failMsg{"Failed to create EventComponent for Widget with ID=\"" + parentID + "\"."};
        return std::unexpected(Error{Error::Signal::InvalidConfiguration, failMsg});
    } // if createComponentResult == failure
    EventComponent* component{createComponentResult.first->second.get()};
    component->handledTypes = handledTypes;
    for(const auto& eventType: handledTypes) {
        // Ensure EventType is valid
        const int eventIndex{static_cast<int>(eventType)};
        if(eventIndex < 0 || eventIndex >= static_cast<int>(Event::Type::NumEventTypes)) {
            const std::string failMsg{"Error: EventType has unexpected value (" + std::to_string(eventIndex) + ")."};
            return std::unexpected(Error{Error::Signal::UnexpectedValue, failMsg});
        } // range-checking for EventType
        // Add EventHandler to the HandlerList
        EventHandlerList& handlerList{GetEventHandlerList(eventType)};
        handlerList.push_back(handler);
    } // for each handled Event::Type
    component->handler = handler;
    return component;
}

std::optional<farcical::engine::Error> farcical::engine::EventSystem::DestroyEventComponent(EntityID parentID) {
    const auto& findComponent{components.find(parentID)};
    if(findComponent == components.end()) {
        const std::string failMsg{
            "Error: Could not find EventComponent for Widget with ID=\"" + parentID +
            "\", which was requested to be destroyed."
        };
        return Error{Error::Signal::InvalidConfiguration, failMsg};
    } // if findComponent == failure
    EventComponent* component{findComponent->second.get()};
    for(const auto& eventType: findComponent->second->handledTypes) {
        // Ensure EventType is valid
        const int eventIndex{static_cast<int>(eventType)};
        if(eventIndex < 0 || eventIndex >= static_cast<int>(Event::Type::NumEventTypes)) {
            const std::string failMsg{"Error: EventType has unexpected value (" + std::to_string(eventIndex) + ")."};
            return Error{Error::Signal::UnexpectedValue, failMsg};
        } // range-checking for EventType
        // Find the appropriate EventHandler on the list
        EventHandlerList& handlerList{GetEventHandlerList(eventType)};
        for(auto iter = handlerList.begin(); iter != handlerList.end(); ++iter) {
            if(*iter == component->handler) {
                handlerList.erase(iter);
                break;
            } // if this iterator matches the component's EventHandler*
        } // for each EventHandler in HandlerList
    } // for each handled Event::Type
    components.erase(findComponent);
    return std::nullopt;
}

farcical::engine::EventSystem::EventHandlerList& farcical::engine::EventSystem::GetEventHandlerList(Event::Type type) const {
    return const_cast<EventHandlerList&>(handlers[static_cast<int>(type)]);
}
