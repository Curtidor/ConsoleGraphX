#include "CGXPCH.h"
#include "system_manager.h"
#include "base_system.h"

namespace ConsoleGraphX_Internal 
{
	SystemManager::~SystemManager()
	{
		for (ConsoleGraphX::BaseSystem* system : _m_systems)
		{
			delete system;
		}
	}

	void SystemManager::Update(float deltaTime)
	{
		for (ConsoleGraphX::BaseSystem* system : _m_systems)
		{
			system->Update(deltaTime);
		}
	}
};