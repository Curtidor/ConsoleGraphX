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

	void SystemManager::RegisterCallTypeSystem(CALLTYPE callType, ConsoleGraphX::BaseSystem* system)
	{
		if (system == nullptr)
		{
			return;
		}
		_m_callTypeSystems[callType].push_back(system);
	}

	void SystemManager::CallSystems(CALLTYPE callType)
	{
		auto it = _m_callTypeSystems.find(callType);
		if (it != _m_callTypeSystems.end())
		{
			for (auto& system : it->second)
			{
				switch (callType)
				{
				case ConsoleGraphX_Internal::EndOfFrame:
					system->EndOfFrame();
				default:
					break;
				}
			}
		}
	}

	void SystemManager::Update(float deltaTime, ConsoleGraphX::SceneSystem& sceneSystem)
	{
		for (auto& systemPair : _m_systems)
		{
			systemPair.second->Update(deltaTime, sceneSystem);
		}
	}
	void SystemManager::ShutDown()
	{
		for (auto& [type_index , base_system] : _m_systems)
		{
			base_system->ShutDown();
		}
	}
};