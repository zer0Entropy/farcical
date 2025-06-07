
#include <cmath>
#include <string>

#include "../include/app.hpp"
#include "../include/ui/menu.hpp"

int main()
{
    farcical::Application app;
    app.Init();

    auto windowSize{app.GetWindow().getSize()};
    auto menu{app.GetUIManager().CreateMenu("MainMenu", app.GetResourceManager())};
    menu->SetSize(windowSize);
    menu->CreateMenuItem("New Game");

    while(app.GetStatus() == farcical::Application::Status::IsRunning) {
      app.Update();
    };
    return 0;
}
