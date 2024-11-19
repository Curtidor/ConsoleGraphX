#pragma once
#include <vector>
#include <string>
#include "scene.h"
#include "scene_system.h"

namespace ConsoleGraphX
{
    class IGameModule
    {
    public:
        virtual ~IGameModule() = default;

        virtual void Initialize() = 0;
        virtual void RegisterScenes(ConsoleGraphX::SceneSystem& system) = 0;
        virtual void LoadInitialScene(ConsoleGraphX::SceneSystem& system) = 0;
    };
};

