#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "Engine\Scene\scene.h"
#include "Engine\Systems\scene_system.h"
#include "Engine\Scene\scene.h"

namespace ConsoleGraphX
{
    class IGameModule
    {
    public:
        virtual ~IGameModule() = default;

        virtual void RegisterScenes(ConsoleGraphX::SceneSystem& system) = 0;
    };
};



namespace ConsoleGraphX
{
    class IGameModuleRegistrar
    {
    public:
        virtual ~IGameModuleRegistrar() = default;

        virtual void RegisterSceneFactory(const std::string& name, std::function<std::unique_ptr<Scene>()> factory) = 0;

        // Optional templated helper
        template <typename SceneType>
        void RegisterSceneFactoryTyped(const std::string& name)
        {
            static_assert(std::is_base_of_v<Scene, SceneType>, "SceneType must inherit from Scene");

            RegisterSceneFactory(name, [name]() {
                return std::make_unique<SceneType>(name);
                });
        }
    };
}
