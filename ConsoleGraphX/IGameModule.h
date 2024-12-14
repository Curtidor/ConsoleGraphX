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

        virtual void RegisterScenes(ConsoleGraphX::SceneSystem& system) = 0;
    };
};

