#include "PCH_CGX.h"
#include "Engine\Components\script.h"
#include "Engine\Systems\script_system.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Resources\Pools\component_script_pool.h"
#include "Engine\Resources\resource_manager.h"

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