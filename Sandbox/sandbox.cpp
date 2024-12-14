#include "../ConsoleGraphX/IGameModule.h"
#include "main_scene.h"
#include "second_scene.h"

/*
RGB(242, 192, 162)
RGB(233, 132, 114)
RGB(216, 35, 35)
RGB(152, 24, 60)
RGB(31, 203, 35)
RGB(18, 109, 48)
RGB(38, 221, 221)
RGB(24, 103, 160)
RGB(147, 66, 38)
RGB(108, 37, 30)
RGB(247, 226, 108)
RGB(237, 179, 41)
RGB(231, 109, 20)
RGB(242, 242, 249)
RGB(106, 95, 160)
RGB(22, 20, 35)
*/


using namespace ConsoleGraphX;

class SandboxGameModule : public IGameModule
{
public:
    void RegisterScenes(ConsoleGraphX::SceneSystem& system) override
    {
        system.RegisterScene(std::make_unique<MainScene>("Main Scene"));
        system.RegisterScene(std::make_unique<SecondScene>("Second Scene"));
    }
};

extern "C" __declspec(dllexport) IGameModule * CreateGameModule()
{
    return new SandboxGameModule();
}
