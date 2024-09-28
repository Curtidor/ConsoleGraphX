#include <vector>
#include "script.h"
#include "script_system.h"
#include "resourcec_manager.h"
#include "component_script_pool.h"

namespace ConsoleGraphX
{
	void ScriptSystem::Initialize() 
	{}

	void ScriptSystem::Update(float deltaTime)
	{
		ConsoleGraphX_Internal::ComponentPoolScript* scriptPool = ConsoleGraphX_Internal::ResourceManager::Instance().GetResourcePool<Script>();
		std::vector<Script*>& scripts = *scriptPool->GetPoolItems();

		for (Script* script : scripts)
		{
			if (script != nullptr && script->IsEnabled())
			{
				script->Update(deltaTime);
			}
		}
	}

	void ScriptSystem::ScriptWarmUp()
	{
		ConsoleGraphX_Internal::ComponentPoolScript* scriptPool = ConsoleGraphX_Internal::ResourceManager::Instance().GetResourcePool<Script>();
		std::vector<Script*>& scripts = *scriptPool->GetPoolItems();

		for (Script* script : scripts)
		{
			if (script != nullptr)
			{
				script->Awake();
			}
		}

		for (Script* script : scripts)
		{

			if (script != nullptr)
			{
				script->Start();
			}
		}
	}
}