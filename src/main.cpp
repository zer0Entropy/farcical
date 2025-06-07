
#include <cmath>
#include <string>

#include "../include/engine.hpp"
#include "../include/ui/menu.hpp"

int main()
{
    farcical::Engine app;
    app.Init();

    auto windowSize{app.GetWindow().getSize()};
    auto menu{app.GetUIManager().CreateMenu("MainMenu", app.GetResourceManager())};
    menu->SetSize(windowSize);
    menu->CreateMenuItem("New Game");

    while(app.GetStatus() == farcical::Engine::Status::IsRunning) {
      app.Update();
    };
    return 0;
}
