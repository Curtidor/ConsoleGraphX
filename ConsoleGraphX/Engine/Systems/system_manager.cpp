#include "PCH_CGX.h"
#include <utility>
#include "Engine\Core\Application\system_manager.h"
#include "Engine\Systems\base_system.h"
#include "Engine\Systems\scene_system.h"

namespace ConsoleGraphX_Internal 
{
	SystemManager::SystemManager() 
	{
		_m_frameExecutor = FrameExecutor();
	}


	SystemManager::~SystemManager()
	{
		for (auto& systemPair : _m_systems)
		{
			delete systemPair.second;
		}
	}


	void SystemManager::RunFrameTimeEvent(FrameExecutionStage stage)
	{
		for (auto& [type_index, base_system] : _m_systems)
		{
			_m_frameExecutor.ExecuteFrameStage(stage, *base_system);
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

