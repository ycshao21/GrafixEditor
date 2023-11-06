#include "Grafix.h"
#include "Grafix/Core/EntryPoint.h"

#include "GameLayer.h"

class Game final : public Grafix::Application
{
public:
    Game(const Grafix::AppSpecification& appSpec)
        : Application(appSpec)
    {
        PushLayer(new GameLayer());
    }

    virtual ~Game() = default;
};

namespace Grafix
{
    Application* CreateApplication()
    {
        AppSpecification appSpec;
        appSpec.Name = "Build Your Dream";
        appSpec.Width = 1440;
        appSpec.Height = 810;

        return new Game(appSpec);
    }
}