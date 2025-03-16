#pragma once
#include <vector>
#include <string>
#include "Engine\Scene\scene.h"
#include "Engine\Systems\scene_system.h"

namespace ConsoleGraphX
{
    class IGameModule
    {
    public:
        virtual ~IGameModule() = default;

        virtual void RegisterScenes(ConsoleGraphX::SceneSystem& system) = 0;
    };
};

