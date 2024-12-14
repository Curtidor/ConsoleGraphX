#include "PCH_CGX.h"
#include "script.h"
#include "script_system.h"
#include "resource_manager.h"
#include "component_script_pool.h"
#include "resource_manager.h"

namespace ConsoleGraphX
{
	void ScriptSystem::Initialize() 
	{}

	void ScriptSystem::Update(float deltaTime, SceneSystem& sceneSystem)
	{
		ConsoleGraphX_Internal::ComponentPoolScript& scriptPool = sceneSystem.GetActiveResourceManager().GetResourcePool<Script>();
		std::vector<Script*>& scripts = *scriptPool.GetPoolItems();

		for (Script* script : scripts)
		{
			if (script != nullptr && script->IsEnabled())
			{
				script->Update(deltaTime);
			}
		}
	}

	void ScriptSystem::ScriptWarmUp(SceneSystem& sceneSystem)
	{
		ConsoleGraphX_Internal::ComponentPoolScript& scriptPool = sceneSystem.GetActiveResourceManager().GetResourcePool<Script>();
		std::vector<Script*>& scripts = *scriptPool.GetPoolItems();

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