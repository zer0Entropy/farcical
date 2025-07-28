//
// Created by dgmuller on 7/24/25.
//
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../../include/ui/factory.hpp"
#include "../../include/game/game.hpp"

std::expected<farcical::ui::Decoration*, farcical::engine::Error> farcical::ui::factory::CreateDecoration(
    const game::Game& game, const DecorationConfig& config, Container* parent) {
    engine::LogSystem& logSystem{game.GetEngine().GetLogSystem()};
    const auto& windowSize{game.GetEngine().GetWindow().getSize()};
    const Scene* currentScene{game.GetCurrentScene()};
    if(!config.id.empty()) {
        // Load Texture from Scene cache
        sf::Texture* texture{currentScene->GetCachedTexture(config.textureProperties.id)};
        const TextureProperties& textureProperties{
            currentScene->GetCachedTextureProperties(config.textureProperties.id)
        };
        if(!texture) {
            const std::string failMsg{"NullPtr Error: Failed to retrieve Texture from Scene cache."};
            return std::unexpected(engine::Error{engine::Error::Signal::NullPtr, failMsg});
        } // if texture == nullptr
        parent->AddChild(std::make_unique<Decoration>(config.id, parent));
        Decoration* decoration{dynamic_cast<Decoration*>(parent->FindChild(config.id))};
        decoration->SetTexture(texture);
        decoration->SetScale(sf::Vector2f{textureProperties.scale, textureProperties.scale});
        sf::Sprite tempSprite{*texture};
        decoration->SetSize(sf::Vector2u{
            static_cast<unsigned int>(tempSprite.getGlobalBounds().size.x * textureProperties.scale),
            static_cast<unsigned int>(tempSprite.getGlobalBounds().size.y * textureProperties.scale)
        });
        // This is a hack!
        // We're centering everything EXCEPT Decorations that have relativePosition == (0, 0)
        if(config.relativePosition.x > 0 && config.relativePosition.y > 0) {
            decoration->SetPosition(sf::Vector2f{
                config.relativePosition.x * windowSize.x / 100.0f - decoration->GetSize().x / 2.0f,
                config.relativePosition.y * windowSize.y / 100.0f - decoration->GetSize().y / 2.0f
            });
        } // if relativePosition != (0, 0)
        return decoration;
    } // if Decoration has valid ID
    const std::string failMsg{"Invalid configuration: Failed to create Decoration (id=" + config.id + ")."};
    return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
}

std::optional<farcical::engine::Error> farcical::ui::factory::DestroyDecoration(
    const game::Game& game, Decoration* decoration) {
    Scene* currentScene{game.GetCurrentScene()};
    const auto& destroyRenderComponent{
        game.GetEngine().GetRenderSystem().DestroyRenderComponent(currentScene->GetID(), decoration->GetID())
    };
    decoration->GetParent()->RemoveChild(decoration->GetID());
    return std::nullopt;
}

std::expected<farcical::ui::Text*, farcical::engine::Error> farcical::ui::factory::CreateText(
    const game::Game& game, const TextConfig& config, Container* parent) {
    engine::LogSystem& logSystem{game.GetEngine().GetLogSystem()};
    const auto& windowSize{game.GetEngine().GetWindow().getSize()};
    const Scene* currentScene{game.GetCurrentScene()};
    if(!config.id.empty()) {
        sf::Font* font{currentScene->GetCachedFont(config.fontProperties.id)};
        const FontProperties& fontProperties{currentScene->GetCachedFontProperties(config.fontProperties.id)};
        parent->AddChild(std::make_unique<Text>(config.id, parent));
        Text* label{dynamic_cast<Text*>(parent->FindChild(config.id))};
        label->SetFont(*font);
        label->SetFontProperties(fontProperties);
        label->SetContents(config.contents);
        label->SetScale(sf::Vector2f{fontProperties.scale, fontProperties.scale});
        const sf::Text tempText{*font, config.contents, fontProperties.characterSize};
        label->SetSize(sf::Vector2u{
            static_cast<unsigned int>(tempText.getGlobalBounds().size.x),
            static_cast<unsigned int>(tempText.getGlobalBounds().size.y)
        });
        label->SetPosition(sf::Vector2f{
            config.relativePosition.x * windowSize.x / 100.0f - label->GetSize().x / 2.0f,
            config.relativePosition.y * windowSize.y / 100.0f
        });
        return label;
    } // if title has valid ID

    const std::string failMsg{"Invalid configuration: Failed to create Text (id=" + config.id + ")."};
    return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
}

std::optional<farcical::engine::Error> farcical::ui::factory::DestroyText(const game::Game& game, Text* text) {
    Scene* currentScene{game.GetCurrentScene()};
    const auto& destroyRenderComponent{
        game.GetEngine().GetRenderSystem().DestroyRenderComponent(currentScene->GetID(), text->GetID())
    };
    text->GetParent()->RemoveChild(text->GetID());
    return std::nullopt;
}

std::expected<farcical::ui::Menu*, farcical::engine::Error> farcical::ui::factory::CreateMenu(
    const game::Game& game,
    const MenuConfig& config,
    Container* parent) {
    engine::LogSystem& logSystem{game.GetEngine().GetLogSystem()};
    const auto& windowSize{game.GetEngine().GetWindow().getSize()};
    const Scene* currentScene{game.GetCurrentScene()};
    if(!config.id.empty()) {
        std::vector<sf::Texture*> buttonTextures{GetButtonTextures(*currentScene, config.buttonConfig)};

        const MenuItemCollection menuItems{CreateMenuItemCollection(config)};

        MenuLayout menuLayout{
            config.layout.orientation,
            config.layout.relativeSpacing
        };
        // Combine relativeSpacing with texture size to calculate actualSpacing
        const TextureProperties& buttonProperties{
            config.buttonConfig.textureProperties[static_cast<int>(Button::Status::Normal)].second
        };
        ResourceID textureID{buttonProperties.id + "Texture"};
        const TextureProperties& textureProperties{currentScene->GetCachedTextureProperties(textureID)};
        const sf::Vector2u& textureSize{
            sf::Vector2u{
                static_cast<unsigned int>(buttonTextures[0]->getSize().x * textureProperties.scale),
                static_cast<unsigned int>(buttonTextures[0]->getSize().y * textureProperties.scale)
            }
        };
        if(config.layout.orientation == MenuLayout::Orientation::Horizontal) {
            menuLayout.actualSpacing = static_cast<float>(textureSize.x * menuLayout.relativeSpacing) / 100.0f;
        } // if orientation == Horizontal
        else if(config.layout.orientation == MenuLayout::Orientation::Vertical) {
            menuLayout.actualSpacing = static_cast<float>(textureSize.y * menuLayout.relativeSpacing) / 100.0f;
        } // else if orientation == Vertical

        if(!parent) {
            parent = &game.GetCurrentScene()->GetRootContainer();
        } // if !parent

        const unsigned int menuIndex{parent->GetNumChildren()};
        parent->AddChild(std::make_unique<Menu>(config.id, menuLayout, parent));
        logSystem.AddMessage("Menu created (id=" + config.id + ").");
        Menu* menu{dynamic_cast<Menu*>(parent->GetChild(menuIndex))};
        sf::Vector2f menuPosition{
            static_cast<float>(windowSize.x * config.relativePosition.x) / 100.0f,
            static_cast<float>(windowSize.y * config.relativePosition.y) / 100.0f
        };
        // Create each MenuItem, then set its Button and Label positions according to the MenuLayout
        for(unsigned int itemIndex = 0; itemIndex < menuItems.ids.size(); ++itemIndex) {
            const auto& createItemResult{
                CreateMenuItem(game, config, config.menuItemConfigs[itemIndex], menu)
            };
            if(!createItemResult.has_value()) {
                const std::string failMsg{
                    "Error: Failed to create MenuItem (id=" + config.menuItemConfigs[itemIndex].id + ")."
                };
                return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
            } // if createItemResult == failure
            MenuItem* item{createItemResult.value()};
            logSystem.AddMessage("MenuItem created (id=" + item->GetID() + ").");
            Button* button{item->GetButton()};
            sf::Vector2f buttonPosition{0.0f, 0.0f};
            Text* label{item->GetLabel()};
            sf::Vector2f labelPosition{0.0f, 0.0f};
            if(menuLayout.orientation == MenuLayout::Orientation::Horizontal) {
                buttonPosition.x = menuPosition.x
                                   + static_cast<float>(itemIndex * button->GetSize().x)
                                   + static_cast<float>(itemIndex) * menuLayout.actualSpacing;
                buttonPosition.y = menuPosition.y + static_cast<float>(button->GetSize().y) / 2.0f;
            } // if layout == Horizontal
            else if(menuLayout.orientation == MenuLayout::Orientation::Vertical) {
                buttonPosition.x = menuPosition.x - static_cast<float>(button->GetSize().x) / 2.0f;
                buttonPosition.y = menuPosition.y
                                   + static_cast<float>(itemIndex * button->GetSize().y)
                                   + static_cast<float>(itemIndex) * menuLayout.actualSpacing;
            } // else if layout == Vertical
            labelPosition.x = buttonPosition.x
                              + static_cast<float>(button->GetSize().x) / 2.0f
                              - static_cast<float>(label->GetSize().x) / 2.0f;
            labelPosition.y = buttonPosition.y
                              + static_cast<float>(button->GetSize().y) / 2.0f
                              - static_cast<float>(label->GetSize().y) / 2.0f;
            button->SetPosition(buttonPosition);
            label->SetPosition(labelPosition);
        } // for each MenuItem

        // Create MenuController
        const auto& createMenuController{
            currentScene->GetRootContainer().CreateMenuController(menu, game.GetEngine().GetEventSystem(),
                                                                  game.GetEngine().GetInputSystem())
        };
        return menu;
    } // if menu has valid ID

    const std::string failMsg{"Invalid configuration: Failed to create Menu (id=" + config.id + ")."};
    return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
}

std::optional<farcical::engine::Error> farcical::ui::factory::DestroyMenu(const game::Game& game, Menu* menu) {
    engine::LogSystem& logSystem{game.GetEngine().GetLogSystem()};
    const Scene* currentScene{game.GetCurrentScene()};

    const auto& destroyMenuController{
        currentScene->GetRootContainer().DestroyMenuController(menu, game.GetEngine().GetInputSystem())
    };
    if(destroyMenuController.has_value()) {
        return destroyMenuController.value();
    } // if destroyMenuController == failure

    const auto& itemList{menu->GetMenuItems()};
    for(const auto& item: itemList) {
        const auto& destroyMenuItem{DestroyMenuItem(game, item)};
        menu->RemoveChild(item->GetID());
    } // for each MenuItem

    menu->GetParent()->RemoveChild(menu->GetID());
    return std::nullopt;
}

std::expected<farcical::ui::MenuItem*, farcical::engine::Error> farcical::ui::factory::CreateMenuItem(
    const game::Game& game, const MenuConfig& menuConfig, const MenuItemConfig& config, Menu* menu) {
    // Create MenuItem
    unsigned int itemIndex{menu->GetNumChildren()};
    menu->AddChild(std::make_unique<MenuItem>(config.id, config.activationEventType, config.activationEventArgs, menu));
    MenuItem* item{dynamic_cast<MenuItem*>(menu->GetChild(itemIndex))};

    // Create Button
    const auto& createButtonResult{
        CreateMenuButton(game, config, menuConfig, item)
    };

    // Create Label
    const auto& createLabelResult{
        CreateMenuLabel(game, config, menuConfig, item)
    };

    return item;
}

std::optional<farcical::engine::Error> farcical::ui::factory::DestroyMenuItem(const game::Game& game, MenuItem* item) {
    engine::RenderSystem& renderSystem{game.GetEngine().GetRenderSystem()};
    engine::EntityID sceneID{game.GetCurrentScene()->GetID()};
    renderSystem.DestroyRenderComponent(sceneID, item->GetButton()->GetID());
    renderSystem.DestroyRenderComponent(sceneID, item->GetLabel()->GetID());
    return std::nullopt;
}

std::expected<farcical::ui::Button*, farcical::engine::Error> farcical::ui::factory::CreateMenuButton(
    const game::Game& game, const MenuItemConfig& itemConfig, const MenuConfig& menuConfig, MenuItem* item) {
    // Create Button
    const engine::EntityID id{itemConfig.id + "Button"};
    item->AddChild(std::make_unique<Button>(id, item));
    Button* button{item->GetButton()};

    // Retrieve cached Textures from currentScene
    const Scene* currentScene{game.GetCurrentScene()};
    std::vector<sf::Texture*> textures{GetButtonTextures(*currentScene, menuConfig.buttonConfig)};

    // Set the appropriate Texture for each Button::Status value
    for(unsigned int statusIndex = static_cast<int>(Button::Status::Normal);
        statusIndex < static_cast<int>(Button::Status::NumStates); ++statusIndex) {
        button->SetTexture(static_cast<Button::Status>(statusIndex), *textures[statusIndex]);
    } // for each ButtonStatus value

    // Retrieve cached TextureProperties from currentScene
    const ResourceID textureID{
        itemConfig.buttonConfig.textureProperties.at(static_cast<int>(button->GetStatus())).second.id + "Texture"
    };
    const TextureProperties& textureProperties{currentScene->GetCachedTextureProperties(textureID)};
    button->SetScale(sf::Vector2f{textureProperties.scale, textureProperties.scale});
    const sf::Sprite tempSprite{*button->GetTexture()};
    button->SetSize(sf::Vector2u{
        static_cast<unsigned int>(tempSprite.getGlobalBounds().size.x * textureProperties.scale),
        static_cast<unsigned int>(tempSprite.getGlobalBounds().size.y * textureProperties.scale)
    });

    return button;
}

std::expected<farcical::ui::Text*, farcical::engine::Error> farcical::ui::factory::CreateMenuLabel(
    const game::Game& game, const MenuItemConfig& itemConfig, const MenuConfig& menuConfig, MenuItem* item) {
    // Create Label
    const engine::EntityID id{itemConfig.id + "Label"};
    item->AddChild(std::make_unique<Text>(id, item));
    Text* label{item->GetLabel()};

    // Retrieve cached Font and FontProperties from currentScene
    const Scene* currentScene{game.GetCurrentScene()};
    const ResourceID fontID{itemConfig.labelConfig.fontProperties.id};
    sf::Font* font{currentScene->GetCachedFont(fontID)};
    const FontProperties& fontProperties{currentScene->GetCachedFontProperties(fontID)};
    label->SetFont(*font);
    label->SetFontProperties(fontProperties);
    label->SetContents(itemConfig.labelConfig.contents);
    label->SetScale(sf::Vector2f{fontProperties.scale, fontProperties.scale});
    sf::Text tempText{*font, std::string{label->GetContents()}, fontProperties.characterSize};
    tempText.setScale(label->GetScale());
    label->SetSize(sf::Vector2u{
        static_cast<unsigned int>(tempText.getGlobalBounds().size.x),
        static_cast<unsigned int>(tempText.getGlobalBounds().size.y)
    });

    return label;
}

farcical::ui::MenuItemCollection farcical::ui::factory::CreateMenuItemCollection(const MenuConfig& config) {
    MenuItemCollection collection;

    for(const auto& itemConfig: config.menuItemConfigs) {
        collection.ids.push_back(itemConfig.id);
        collection.contents.push_back(itemConfig.labelConfig.contents);
        collection.eventTypes.push_back(itemConfig.activationEventType);
        collection.eventArgs.push_back(itemConfig.activationEventArgs);
    } // for each menuItem

    return collection;
}

std::vector<sf::Texture*> farcical::ui::factory::GetButtonTextures(const Scene& scene, const ButtonConfig& config) {
    std::vector<sf::Texture*> textures;
    for(const auto& textureProperties: config.textureProperties | std::views::values) {
        const ResourceID textureID{textureProperties.id + "Texture"};
        sf::Texture* texture{scene.GetCachedTexture(textureID)};
        if(texture) {
            textures.push_back(texture);
        } // if texture
    } // for each texture in buttonConfig
    return textures;
}
