#include "PCH_CGX.h"
#include "Engine\Components\script.h"
#include "Engine\Systems\script_system.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Resources\Pools\component_script_pool.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Core\Application\update_context.h"
#include "Engine\Systems\input_system.h"


namespace ConsoleGraphX
{
	void ScriptSystem::Initialize() 
	{}

	void ScriptSystem::Update(float deltaTime, SceneSystem& sceneSystem)
	{
		ConsoleGraphX_Internal::ComponentPoolScript& scriptPool = sceneSystem.GetActiveResourceManager().GetResourcePool<Script>();
		std::vector<Script*>& scripts = *scriptPool.GetPoolItems();

		UpdateContext context{sceneSystem,  deltaTime, InputSystem::Instance().GetInputSnapshot() };

		for (Script* script : scripts)
		{
			if (script != nullptr && script->IsEnabled())
			{
				script->Update(context);
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