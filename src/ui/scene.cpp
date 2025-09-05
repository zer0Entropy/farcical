//
// Created by dgmuller on 6/24/25.
//
#include "../../include/ui/scene.hpp"

farcical::ui::Scene::Scene(engine::EntityID id):
    Container(id, Widget::Type::Scene, nullptr),
    focusedWidget{nullptr} {
}

farcical::ui::Widget* farcical::ui::Scene::GetFocusedWidget() const {
    return focusedWidget;
}

void farcical::ui::Scene::SetFocusedWidget(Widget* widget) {
    if(focusedWidget) {
        focusedWidget->DoAction(Action{Action::Type::LoseFocus});
    } // if focusedWidget
    focusedWidget = widget;
    if(focusedWidget) {
        focusedWidget->DoAction(Action{Action::Type::ReceiveFocus});
    } // if focusedWidget
}

void farcical::ui::Scene::DoAction(Action action) {
    const auto& focusList{this->GetFocusList()};
    int mod{0};
    if(action.type == Action::Type::MoveFocusUp) {
        mod = -1;
    } // if MoveFocusUp
    else if(action.type == Action::Type::MoveFocusDown) {
        mod = 1;
    } // else if MoveFocusDown
    if(mod != 0) {
        if(focusedWidget) {
            int index{0};
            for(const auto& widget: focusList) {
                if(widget == focusedWidget) {
                    int focusIndex = index + mod;
                    if(focusIndex < 0) {
                        focusIndex = static_cast<int>(focusList.size()) - 1;
                    } // if focusIndex < 0
                    else if(focusIndex >= focusList.size()) {
                        focusIndex = 0;
                    } // else if focusIndex >= focusList.size()
                    SetFocusedWidget(focusList.at(focusIndex));
                    break;
                } // if this Widget is currently focused
                ++index;
            } // for each Widget in focusList
        } // if focusedWidget
        else {
            if(action.type == Action::Type::MoveFocusUp) {
                SetFocusedWidget(*(focusList.end() - 1));
            } // if MoveFocusUp
            else if(action.type == Action::Type::MoveFocusDown) {
                SetFocusedWidget(*focusList.begin());
            } // else if MoveFocusDown
        } // else !focusedWidget
    } // if MoveFocusUp or MoveFocusDown

    else if(action.type == Action::Type::ConfirmSelection) {

    } // else if ConfirmSelection
}

void farcical::ui::Scene::HandleEvent(const engine::Event& event) {
    if(event.type == engine::Event::Type::SetFocus) {
        const ResourceID widgetID{std::any_cast<std::string>(event.args.at(0))};
        Widget* widget{this->FindChild(widgetID)};
        if(widget) {
            SetFocusedWidget(widget);
        } // if widget
    } // if event.type == SetFocus
}

sf::Music* farcical::ui::Scene::GetCachedMusic(ResourceID id) const {
    const auto& findMusic{musicCache.find(id)};
    if(findMusic != musicCache.end()) {
        return findMusic->second;
    }
    return nullptr;
}

farcical::MusicProperties farcical::ui::Scene::GetCachedMusicProperties(ResourceID id) const {
    const auto& findProperties{musicPropertiesCache.find(id)};
    if(findProperties != musicPropertiesCache.end()) {
        return findProperties->second;
    }
    return MusicProperties{};
}

sf::Font* farcical::ui::Scene::GetCachedFont(ResourceID id) const {
    const auto& findFont{fontCache.find(id)};
    if(findFont != fontCache.end()) {
        return findFont->second;
    }
    return nullptr;
}

farcical::FontProperties farcical::ui::Scene::GetCachedFontProperties(ResourceID id) const {
    const auto& findProperties{fontPropertiesCache.find(id)};
    if(findProperties != fontPropertiesCache.end()) {
        return findProperties->second;
    }
    return FontProperties{};
}

sf::Texture* farcical::ui::Scene::GetCachedTexture(ResourceID id) const {
    const auto& findTexture{textureCache.find(id)};
    if(findTexture != textureCache.end()) {
        return findTexture->second;
    }
    return nullptr;
}

farcical::TextureProperties farcical::ui::Scene::GetCachedTextureProperties(ResourceID id) const {
    const auto& findProperties{texturePropertiesCache.find(id)};
    if(findProperties != texturePropertiesCache.end()) {
        return findProperties->second;
    }
    return TextureProperties{};
}

void farcical::ui::Scene::CacheMusic(ResourceID id, sf::Music* music) {
    musicCache.emplace(id, music);
}

void farcical::ui::Scene::CacheMusicProperties(ResourceID id, const MusicProperties& musicProperties) {
    musicPropertiesCache.emplace(id, musicProperties);
}

void farcical::ui::Scene::CacheFont(ResourceID id, sf::Font* font) {
    fontCache.emplace(id, font);
}

void farcical::ui::Scene::CacheFontProperties(ResourceID id, const FontProperties& fontProperties) {
    fontPropertiesCache.emplace(id, fontProperties);
}

void farcical::ui::Scene::CacheTexture(ResourceID id, sf::Texture* texture) {
    textureCache.emplace(id, texture);
}

void farcical::ui::Scene::CacheTextureProperties(ResourceID id, const TextureProperties& textureProperties) {
    texturePropertiesCache.emplace(id, textureProperties);
}

void farcical::ui::Scene::ClearMusicCache() {
    musicCache.clear();
}

void farcical::ui::Scene::ClearMusicPropertiesCache() {
    musicPropertiesCache.clear();
}

void farcical::ui::Scene::ClearFontCache() {
    fontCache.clear();
}

void farcical::ui::Scene::ClearFontPropertiesCache() {
    fontPropertiesCache.clear();
}

void farcical::ui::Scene::ClearTextureCache() {
    textureCache.clear();
}

void farcical::ui::Scene::ClearTexturePropertiesCache() {
    texturePropertiesCache.clear();
}
/*
std::expected<farcical::ui::MenuController*, farcical::engine::Error> farcical::ui::Scene::CreateMenuController(
    Menu* menu, engine::EventSystem& eventSystem) {
    const auto& createMenuController{
        menuControllers.insert(std::make_pair(menu->GetID(), std::make_unique<MenuController>(menu, eventSystem)))
    };
    if(!createMenuController.second) {
        const std::string failMsg{"Error: Failed to create MenuController for Menu with id=\"" + menu->GetID() + "\"."};
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if createMenuController == failure
    return createMenuController.first->second.get();
}

std::optional<farcical::engine::Error> farcical::ui::Scene::DestroyMenuController(
    Menu* menu, engine::EventSystem& eventSystem) {
    const auto& findMenuController{menuControllers.find(menu->GetID())};
    if(findMenuController == menuControllers.end()) {
        const std::string failMsg{"Error: MenuController for Menu with id=\"" + menu->GetID() + "\" not found."};
        return engine::Error{engine::Error::Signal::ResourceNotFound, failMsg};
    } // if findMenuController == failure
    menuControllers.erase(findMenuController);
    return std::nullopt;
}

farcical::ui::MenuController* farcical::ui::Scene::GetMenuController(engine::EntityID menuID) {
    MenuController* controller{nullptr};
    const auto& findController{menuControllers.find(menuID)};
    if(findController != menuControllers.end()) {
        controller = findController->second.get();
    } // if controller found
    return controller;
}
*/