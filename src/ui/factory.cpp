//
// Created by dgmuller on 7/24/25.
//
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../../include/ui/factory.hpp"
#include "../../include/game/game.hpp"

std::expected<farcical::ui::Decoration*, farcical::engine::Error> farcical::ui::factory::CreateDecoration(
    engine::RenderSystem& renderSystem, Scene* scene, const DecorationProperties& properties) {
    Decoration* decoration{nullptr};
    if(properties.id.empty()) {
        return decoration;
    } // return nullptr if there is no valid ID
    Container* parent{dynamic_cast<Container*>(scene->FindChild(properties.parentID))};
    if(!parent) {
        const std::string failMsg{
            "Invalid configuration: Decoration with ID=" + properties.id + " has parentID=" + properties.parentID +
            ", but no such entity exists."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if parent not found

    // Create Decoration
    parent->AddChild(std::make_unique<Decoration>(properties.id, parent));

    // Add its Texture
    decoration = dynamic_cast<Decoration*>(parent->FindChild(properties.id));
    sf::Texture* texture{scene->GetCachedTexture(properties.textureProperties.id)};
    decoration->SetTexture(texture);

    // Configure its properties
    const TextureProperties& textureProperties{
        scene->GetCachedTextureProperties(properties.textureProperties.id)
    };
    decoration->SetScale(sf::Vector2f{textureProperties.scale, textureProperties.scale});
    sf::Sprite tempSprite{*texture};
    decoration->SetSize(sf::Vector2u{
        static_cast<unsigned int>(tempSprite.getGlobalBounds().size.x * textureProperties.scale),
        static_cast<unsigned int>(tempSprite.getGlobalBounds().size.y * textureProperties.scale)
    });

    const auto& windowSize{renderSystem.GetWindow().getSize()};
    // Centering for any object with a relativePosition != (0, 0)
    if(properties.relativePosition.x > 0 && properties.relativePosition.y > 0) {
        decoration->SetPosition(sf::Vector2f{
            static_cast<float>(properties.relativePosition.x * windowSize.x) / 100.0f
            - static_cast<float>(decoration->GetSize().x) / 2.0f,
            static_cast<float>(properties.relativePosition.y * windowSize.y) / 100.0f
            - static_cast<float>(decoration->GetSize().y) / 2.0f
        });
    } // if relativePosition != (0, 0)

    const auto& createRenderCmp{
        renderSystem.CreateRenderComponent(
            properties.layerID,
            scene->GetID(),
            decoration->GetID(),
            decoration->GetTexture())
    };
    if(createRenderCmp.has_value()) {
        engine::RenderComponent* renderCmp{createRenderCmp.value()};
        renderCmp->scale = decoration->GetScale();
        renderCmp->position = decoration->GetPosition();
        decoration->AddComponent(renderCmp);
    } // if createRenderCmp == success

    return decoration;
}

std::optional<farcical::engine::Error> farcical::ui::factory::DestroyDecoration(
    engine::RenderSystem& renderSystem, Scene* scene, Decoration* decoration) {
    const auto& destroyRenderComponent{
        renderSystem.DestroyRenderComponent(scene->GetID(), decoration->GetID())
    };
    if(destroyRenderComponent.has_value()) {
        return destroyRenderComponent.value();
    } // if destroyRenderComponent == failure
    decoration->GetParent()->RemoveChild(decoration->GetID());
    return std::nullopt;
}

std::expected<farcical::ui::Text*, farcical::engine::Error> farcical::ui::factory::CreateText(
    engine::RenderSystem& renderSystem, Scene* scene, const WidgetProperties& properties, sf::FloatRect bounds) {
    Text* text{nullptr};
    if(properties.id.empty()) {
        return text;
    } // return nullptr if there is no valid ID
    Container* parent{dynamic_cast<Container*>(scene->FindChild(properties.parentID))};
    if(!parent) {
        const std::string failMsg{
            "Invalid configuration: Text with ID=" + properties.id + " has parentID=" + properties.parentID +
            ", but no such entity exists."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if parent not found

    // Create Text
    parent->AddChild(std::make_unique<Text>(properties.id, parent));

    // Add its Font
    text = dynamic_cast<Text*>(parent->FindChild(properties.id));
    const FontProperties& fontProperties{scene->GetCachedFontProperties(properties.labelProperties.second.id)};
    sf::Font* font{scene->GetCachedFont(fontProperties.id)};
    text->SetFont(*font);

    // Configure its properties
    text->SetFontProperties(fontProperties);
    text->SetContents(properties.labelProperties.first);
    text->SetScale(sf::Vector2f{properties.labelProperties.second.scale, properties.labelProperties.second.scale});
    const sf::Text tempText{*font, properties.labelProperties.first, fontProperties.characterSize};
    text->SetSize(sf::Vector2u{
        static_cast<unsigned int>(tempText.getGlobalBounds().size.x * fontProperties.scale),
        static_cast<unsigned int>(tempText.getGlobalBounds().size.y * fontProperties.scale)
    });
    if(bounds.size.x > 0 && bounds.size.y > 0) {
        text->SetPosition(sf::Vector2f{
            bounds.position.x + bounds.size.x / 2.0f - static_cast<float>(text->GetSize().x) / 2.0f,
            bounds.position.y + bounds.size.y / 2.0f - static_cast<float>(text->GetSize().y) / 2.0f
        });
    } // if bounds.size > 0
    else {
        const auto& windowSize{renderSystem.GetWindow().getSize()};
        text->SetPosition(sf::Vector2f{
            static_cast<float>(properties.relativePosition.x * windowSize.x) / 100.0f
            - static_cast<float>(text->GetSize().x) / 2.0f,
            static_cast<float>(properties.relativePosition.y * windowSize.y) / 100.0f
        });
    }

    const auto& createRenderCmp{
        renderSystem.CreateRenderComponent(
            properties.layerID,
            scene->GetID(),
            text->GetID(),
            text->GetFont(),
            text->GetFontProperties(),
            text->GetContents())
    };
    if(createRenderCmp.has_value()) {
        engine::RenderComponent* renderCmp{createRenderCmp.value()};
        renderCmp->scale = text->GetScale();
        renderCmp->position = text->GetPosition();
        text->AddComponent(renderCmp);
    } // if createRenderCmp == success

    return text;
}

std::expected<farcical::ui::Menu*, farcical::engine::Error> farcical::ui::factory::CreateMenu(
    engine::RenderSystem& renderSystem,
    engine::InputSystem& inputSystem,
    engine::EventSystem& eventSystem,
    Scene* scene,
    const MenuProperties& properties) {
    Menu* menu{nullptr};
    if(properties.id.empty()) {
        return menu;
    } // return nullptr if there is no valid ID
    Container* parent{dynamic_cast<Container*>(scene->FindChild(properties.parentID))};
    if(!parent) {
        const std::string failMsg{
            "Invalid configuration: Menu with ID=" + properties.id + " has parentID=" + properties.parentID +
            ", but no such entity exists."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // if parent not found

    // Create Menu
    parent->AddChild(std::make_unique<Menu>(properties.id, properties.menuType, properties.layout, parent));
    menu = dynamic_cast<Menu*>(parent->FindChild(properties.id));

    const auto& windowSize{renderSystem.GetWindow().getSize()};
    menu->SetPosition(sf::Vector2f{
        static_cast<float>(windowSize.x * properties.relativePosition.x) / 100.0f,
        static_cast<float>(windowSize.y * properties.relativePosition.y) / 100.0f
    });

    // Create Menu Items
    if(properties.menuType == Menu::Type::Button) {
        for(auto buttonProperties: properties.buttonProperties) {
            buttonProperties.labelProperties.second = properties.labelProperties.second;
            // Create Button
            const auto& createButtonResult{
                CreateButton(renderSystem, eventSystem, inputSystem, scene, menu, properties, buttonProperties)
            };
            if(!createButtonResult.has_value()) {
                return std::unexpected(createButtonResult.error());
            } // if createButtonResult == failure
            Button* button{createButtonResult.value()};

            // Create Label
            const WidgetProperties labelProperties{
                buttonProperties.id + "Label",
                Widget::Type::Text,
                properties.id,
                Layout::Layer::ID::Foreground,
                buttonProperties.relativePosition,
                buttonProperties.labelProperties
            };
            sf::FloatRect buttonRect{
                button->GetPosition(),
                {
                    static_cast<float>(button->GetSize().x),
                    static_cast<float>(button->GetSize().y)
                }
            };
            const auto& createLabelResult{
                CreateText(renderSystem, scene, labelProperties, buttonRect)
            };
            if(!createLabelResult.has_value()) {
                return std::unexpected(createLabelResult.error());
            } // if createLabelResult == failure
            Text* label{createLabelResult.value()};
        } // for each Button in Menu
    } // if Button Menu

    else if(properties.menuType == Menu::Type::RadioButton) {
        for(auto radioButtonProperties: properties.radioButtonProperties) {
            radioButtonProperties.labelProperties.second = properties.labelProperties.second;
            // Create RadioButton
            const auto& createRadioButtonResult{
                CreateRadioButton(renderSystem, eventSystem, inputSystem, scene, menu, properties,
                                  radioButtonProperties)
            };
            if(!createRadioButtonResult.has_value()) {
                return std::unexpected(createRadioButtonResult.error());
            } // if createRadioButton == failure
            RadioButton* radioButton{createRadioButtonResult.value()};

            // Create Label
            const WidgetProperties labelProperties{
                radioButtonProperties.id + "Label",
                Widget::Type::Text,
                properties.id,
                Layout::Layer::ID::Foreground,
                radioButtonProperties.relativePosition,
                radioButtonProperties.labelProperties
            };
            sf::FloatRect radioButtonRect{
                radioButton->GetPosition(),
                {
                    static_cast<float>(radioButton->GetSize().x),
                    static_cast<float>(radioButton->GetSize().y)
                }
            };
            const auto& createLabelResult{
                CreateText(renderSystem, scene, labelProperties, radioButtonRect)
            };
            if(!createLabelResult.has_value()) {
                return std::unexpected(createLabelResult.error());
            } // if createLabelResult == failure
            Text* label{createLabelResult.value()};
            const sf::Vector2f labelPosition{
                static_cast<float>(radioButton->GetPosition().x + 1.5f * static_cast<float>(radioButton->GetSize().x)),
                static_cast<float>(radioButton->GetPosition().y + static_cast<float>(radioButton->GetSize().y) / 2.0f
                                   - static_cast<float>(label->GetSize().y) / 2.0f)
            };
            label->SetPosition(labelPosition);
            engine::RenderComponent* renderCmp{
                dynamic_cast<engine::RenderComponent*>(label->GetComponent(engine::Component::Type::Render))
            };
            renderCmp->position = labelPosition;
        } // for each RadioButton in Menu
    } // else if RadioButton Menu

    else if(properties.menuType == Menu::Type::SubMenu) {
        for(auto subMenuProperties: properties.menuProperties) {
            subMenuProperties.layerID = properties.layerID;
            const auto& createSubMenu{
                CreateMenu(renderSystem, inputSystem, eventSystem, scene, subMenuProperties)
            };
            if(!createSubMenu.has_value()) {
                return std::unexpected(createSubMenu.error());
            } // if createSubMenu == failure
        } // for each subMenu
    } // else if SubMenu Menu
/*
    // Create MenuController for this Menu
    const auto& createMenuController{
        scene->CreateMenuController(menu, eventSystem)
    };
    if(!createMenuController.has_value()) {
        return std::unexpected(createMenuController.error());
    } // if createMenuController == failure
    MenuController* controller{createMenuController.value()};
    const auto& createInputComponent{
        inputSystem.CreateInputComponent(controller, controller, controller->GetID())
    };
    if(!createInputComponent.has_value()) {
        return std::unexpected(createInputComponent.error());
    } // if createInputComponent == failure
    */
    return menu;
}

std::optional<farcical::engine::Error> farcical::ui::factory::DestroyMenu(
    engine::RenderSystem& renderSystem,
    engine::InputSystem& inputSystem,
    engine::EventSystem& eventSystem,
    Scene* scene,
    Menu* menu) {
    /*
    MenuController* controller{scene->GetMenuController(menu->GetID())};
    const auto& destroyInputCmp{
        inputSystem.DestroyInputComponent(controller->GetID())
    };
    if(destroyInputCmp.has_value()) {
        return destroyInputCmp.value();
    } // if destroyInputComponent == failure
    const auto& destroyMenuController{
        scene->DestroyMenuController(menu, eventSystem)
    };
    if(destroyMenuController.has_value()) {
        return destroyMenuController.value();
    } // if destroyMenuController == failure
    */
    for(const auto& child: menu->GetChildren()) {
        if(child->HasComponent(engine::Component::Type::Render)) {
            const auto& destroyRenderCmp{
                renderSystem.DestroyRenderComponent(scene->GetID(), child->GetID())
            };
            if(destroyRenderCmp.has_value()) {
                return destroyRenderCmp.value();
            } // if destroyRenderCmp == failure
        } // if child has RenderComponent
        if(child->HasComponent(engine::Component::Type::Input)) {
            const auto& destroyInputCmp{
                inputSystem.DestroyInputComponent(child->GetID())
            };
            if(destroyInputCmp.has_value()) {
                return destroyInputCmp.value();
            } // if destroyInputCmp == failure
        } // if child has InputComponent
        if(child->GetType() != Widget::Type::Menu) {
            menu->RemoveChild(child->GetID());
        } // if child is not a SubMenu
        else {
            DestroyMenu(renderSystem, inputSystem, eventSystem, scene, dynamic_cast<Menu*>(child));
        } // else if child is a SubMenu
    } // for each child in Menu

    menu->GetParent()->RemoveChild(menu->GetID());
    return std::nullopt;
}

std::expected<farcical::ui::Button*, farcical::engine::Error> farcical::ui::factory::CreateButton(
    engine::RenderSystem& renderSystem,
    engine::EventSystem& eventSystem,
    engine::InputSystem& inputSystem,
    Scene* scene,
    Menu* menu,
    const MenuProperties& menuProperties,
    const ButtonProperties& buttonProperties) {
    if(buttonProperties.id.empty()) {
        return nullptr;
    } // return nullptr if ID is empty

    // Create Button
    engine::EntityID buttonID{buttonProperties.id + "Button"};
    menu->AddChild(std::make_unique<Button>(buttonID, buttonProperties.onPressEvent, eventSystem, menu));
    Button* button{dynamic_cast<Button*>(menu->FindChild(buttonID))};

    // Get Button index
    int buttonIndex{0};
    for(const auto& menuButton: menuProperties.buttonProperties) {
        if(menuButton.id == buttonProperties.id) {
            break;
        } // if ID's match, break
        ++buttonIndex;
    } // for each Button in Menu

    // Get Textures and TextureProperties
    const std::vector<sf::Texture*> buttonTextures{GetButtonTextures(*scene, menuProperties)};
    const ResourceID textureID{
        menuProperties.buttonTextures.at(static_cast<int>(Button::Status::Normal)).second.id + "Texture"
    };
    const TextureProperties& textureProperties{scene->GetCachedTextureProperties(textureID)};
    int textureIndex{0};
    for(const auto& texture: buttonTextures) {
        Button::Status status{menuProperties.buttonTextures[textureIndex++].first};
        button->SetTexture(status, *texture);
    } // for each Texture

    // Set scale and size
    sf::Vector2f scale{textureProperties.scale, textureProperties.scale};
    button->SetScale(scale);
    const sf::Vector2u size{
        static_cast<unsigned int>(static_cast<float>(buttonTextures[0]->getSize().x) * textureProperties.scale),
        static_cast<unsigned int>(static_cast<float>(buttonTextures[0]->getSize().y) * textureProperties.scale)
    };
    button->SetSize(size);

    // Calculate spacing
    const auto& windowSize{renderSystem.GetWindow().getSize()};
    float spacing{0.0f};
    if(menuProperties.layout.orientation == Orientation::Horizontal) {
        spacing = static_cast<float>(size.x) * (static_cast<float>(menuProperties.layout.relativeSpacing) / 100.0f);
    } // if orientation == Horizontal
    else if(menuProperties.layout.orientation == Orientation::Vertical) {
        spacing = static_cast<float>(size.y) * (static_cast<float>(menuProperties.layout.relativeSpacing) / 100.0f);
    } // else if orientation == Vertical

    // Calculate Position
    sf::Vector2f buttonPosition{0.0f, 0.0f};
    if(menuProperties.layout.orientation == Orientation::Horizontal) {
        buttonPosition.x = menu->GetPosition().x
                           + static_cast<float>(buttonIndex * size.x)
                           + static_cast<float>(buttonIndex) * spacing;
        buttonPosition.y = menu->GetPosition().y + static_cast<float>(size.y) / 2.0f;
    } // if layout == Horizontal
    else if(menuProperties.layout.orientation == Orientation::Vertical) {
        buttonPosition.x = menu->GetPosition().x - static_cast<float>(size.x) / 2.0f;
        buttonPosition.y = menu->GetPosition().y
                           + static_cast<float>(buttonIndex * button->GetSize().y)
                           + static_cast<float>(buttonIndex) * spacing;
    } // else if layout == Vertical
    button->SetPosition(buttonPosition);

    const auto& createRenderCmp{
        renderSystem.CreateRenderComponent(
            menuProperties.layerID,
            scene->GetID(),
            button->GetID(),
            button->GetTexture())
    };
    if(createRenderCmp.has_value()) {
        engine::RenderComponent* renderCmp{createRenderCmp.value()};
        renderCmp->scale = button->GetScale();
        renderCmp->position = button->GetPosition();
        button->AddComponent(renderCmp);
    } // if createRenderCmp == success
    else {
        const std::string failMsg{
            "Error: Failed to create RenderComponent for Button (id=\"" + button->GetID() + "\")."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // else createRenderCmp == failure

    Button::Controller* controller{button->GetController()};
    const auto& createInputCmp{
        inputSystem.CreateInputComponent(controller, controller, button->GetID())
    };
    if(createInputCmp.has_value()) {
        engine::InputComponent* inputCmp{createInputCmp.value()};
        button->AddComponent(inputCmp);
    } // if createInputCmp == success
    else {
        const std::string failMsg{
            "Error: Failed to create InputComponent for Button (id=\"" + button->GetID() + "\")."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // else createInputCmp == failure

    return button;
}

std::expected<farcical::ui::RadioButton*, farcical::engine::Error> farcical::ui::factory::CreateRadioButton(
    engine::RenderSystem& renderSystem,
    engine::EventSystem& eventSystem,
    engine::InputSystem& inputSystem,
    Scene* scene,
    Menu* menu,
    const MenuProperties& menuProperties,
    const WidgetProperties& radioButtonProperties) {
    if(radioButtonProperties.id.empty()) {
        return nullptr;
    } // return nullptr if ID is empty

    // Create RadioButton
    engine::EntityID radioButtonID{radioButtonProperties.id + "RadioButton"};
    menu->AddChild(std::make_unique<RadioButton>(radioButtonID, eventSystem, menu));
    RadioButton* radioButton{dynamic_cast<RadioButton*>(menu->FindChild(radioButtonID))};

    // Get RadioButton index
    int radioButtonIndex{0};
    for(const auto& menuRadioButton: menuProperties.radioButtonProperties) {
        if(menuRadioButton.id == radioButtonProperties.id) {
            break;
        } // if ID's match, break
        ++radioButtonIndex;
    } // for each RadioButton in Menu

    // Get Textures and TextureProperties
    const std::vector<sf::Texture*> radioButtonTextures{GetRadioButtonTextures(*scene, menuProperties)};
    const ResourceID textureID{
        menuProperties.radioButtonTextures.at(static_cast<int>(RadioButton::Status::Off)).second.id + "Texture"
    };
    const TextureProperties& textureProperties{scene->GetCachedTextureProperties(textureID)};
    int textureIndex{0};
    for(const auto& texture: radioButtonTextures) {
        RadioButton::Status status{menuProperties.radioButtonTextures[textureIndex++].first};
        radioButton->SetTexture(status, *texture);
    } // for each Texture

    // Set scale and size
    sf::Vector2f scale{textureProperties.scale, textureProperties.scale};
    radioButton->SetScale(scale);
    const sf::Vector2u size{
        static_cast<unsigned int>(static_cast<float>(radioButtonTextures[0]->getSize().x) * textureProperties.scale),
        static_cast<unsigned int>(static_cast<float>(radioButtonTextures[0]->getSize().y) * textureProperties.scale)
    };
    radioButton->SetSize(size);

    // Calculate spacing
    const auto& windowSize{renderSystem.GetWindow().getSize()};
    float spacing{0.0f};
    if(menuProperties.layout.orientation == Orientation::Horizontal) {
        spacing = static_cast<float>(size.x) * (static_cast<float>(menuProperties.layout.relativeSpacing) / 100.0f);
    } // if orientation == Horizontal
    else if(menuProperties.layout.orientation == Orientation::Vertical) {
        spacing = static_cast<float>(size.y) * (static_cast<float>(menuProperties.layout.relativeSpacing) / 100.0f);
    } // else if orientation == Vertical

    // Calculate Position
    sf::Vector2f radioButtonPosition{0.0f, 0.0f};
    if(menuProperties.layout.orientation == Orientation::Horizontal) {
        radioButtonPosition.x = menu->GetPosition().x
                                + static_cast<float>(radioButtonIndex * size.x)
                                + static_cast<float>(radioButtonIndex) * spacing;
        radioButtonPosition.y = menu->GetPosition().y + static_cast<float>(size.y) / 2.0f;
    } // if layout == Horizontal
    else if(menuProperties.layout.orientation == Orientation::Vertical) {
        radioButtonPosition.x = menu->GetPosition().x - static_cast<float>(size.x) / 2.0f;
        radioButtonPosition.y = menu->GetPosition().y
                                + static_cast<float>(radioButtonIndex * radioButton->GetSize().y)
                                + static_cast<float>(radioButtonIndex) * spacing;
    } // else if layout == Vertical
    radioButton->SetPosition(radioButtonPosition);

    const auto& createRenderCmp{
        renderSystem.CreateRenderComponent(
            menuProperties.layerID,
            scene->GetID(),
            radioButton->GetID(),
            radioButton->GetTexture())
    };
    if(createRenderCmp.has_value()) {
        engine::RenderComponent* renderCmp{createRenderCmp.value()};
        renderCmp->scale = radioButton->GetScale();
        renderCmp->position = radioButton->GetPosition();
        radioButton->AddComponent(renderCmp);
    } // if createRenderCmp == success

    RadioButton::Controller* controller{radioButton->GetController()};
    const auto& createInputCmp{
        inputSystem.CreateInputComponent(controller, controller, radioButton->GetID())
    };
    if(createInputCmp.has_value()) {
        engine::InputComponent* inputCmp{createInputCmp.value()};
        radioButton->AddComponent(inputCmp);
    } // if createInputCmp == success
    else {
        const std::string failMsg{
            "Error: Failed to create InputComponent for RadioButton (id=\"" + radioButton->GetID() + "\")."
        };
        return std::unexpected(engine::Error{engine::Error::Signal::InvalidConfiguration, failMsg});
    } // else createInputCmp == failure

    return radioButton;
}

std::vector<sf::Texture*>
farcical::ui::factory::GetButtonTextures(const Scene& scene, const MenuProperties& properties) {
    std::vector<sf::Texture*> textures;
    for(const auto& textureProperties: properties.buttonTextures | std::views::values) {
        const ResourceID textureID{textureProperties.id + "Texture"};
        sf::Texture* texture{scene.GetCachedTexture(textureID)};
        if(texture) {
            textures.push_back(texture);
        } // if texture
    } // for each buttonTexture in Menu
    return textures;
}

std::vector<sf::Texture*> farcical::ui::factory::GetRadioButtonTextures(const Scene& scene, const MenuProperties& properties) {
    std::vector<sf::Texture*> textures;
    for(const auto& textureProperties: properties.radioButtonTextures | std::views::values) {
        const ResourceID textureID{textureProperties.id + "Texture"};
        sf::Texture* texture{scene.GetCachedTexture(textureID)};
        if(texture) {
            textures.push_back(texture);
        } // if texture
    } // for each radioButtonTexture in Menu
    return textures;
}
