#include "PCH_CGX.h"
#include "Engine\Core\Application\system_manager.h"
#include "Engine\Systems\base_system.h"

namespace ConsoleGraphX_Internal 
{
	SystemManager::~SystemManager()
	{
		for (ConsoleGraphX::BaseSystem* system : _m_systems)
		{
			delete system;
		}
	}

	void SystemManager::Update(float deltaTime, ConsoleGraphX::SceneSystem& sceneSystem)
	{
		for (ConsoleGraphX::BaseSystem* system : _m_systems)
		{
			system->Update(deltaTime, sceneSystem);
		}
	}
};