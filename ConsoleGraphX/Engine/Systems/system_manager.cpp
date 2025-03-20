#include "PCH_CGX.h"
#include <utility>
#include "Engine\Core\Application\system_manager.h"
#include "Engine\Systems\base_system.h"
#include "Engine\Systems\scene_system.h"

namespace ConsoleGraphX_Internal 
{
	SystemManager::~SystemManager()
	{
		for (auto& systemPair : _m_systems)
		{
			delete systemPair.second;
		}
	}

	void SystemManager::Update(float deltaTime, ConsoleGraphX::SceneSystem& sceneSystem)
	{
		for (auto& systemPair : _m_systems)
		{
			systemPair.second->Update(deltaTime, sceneSystem);
		}
	}
};