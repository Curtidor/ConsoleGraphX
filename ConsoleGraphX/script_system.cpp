#include <string>
#include <unordered_set>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "component.h"
#include "dispatcher.h"
#include "entity.h"
#include "script.h"
#include "script_system.h"
#include "verify_macro.h"

namespace ConsoleGraphX
{
	std::unordered_set<Entity*> ScriptSystem::_s_scripts;

	// TODO when script state changes we should stop calling update on that script or start calling update deping on the state

	void ScriptSystem::Initialize() 
	{
		std::string objectName = typeid(Script).name();

		ConsoleGraphX_Interal::Dispatcher<Entity*>::RegisterListener("AddComponent" + objectName, RegisterScript);
		// TODO need to add support for removing single scripts
		ConsoleGraphX_Interal::Dispatcher<Entity*>::RegisterListener("RemoveComponent" + objectName, DeregisterScript);
		ConsoleGraphX_Interal::Dispatcher<Entity*>::RegisterListener("RunTimeScriptAddition" + objectName, RunTimeRegisterScript);
	}

	void ScriptSystem::Update(float delta_time)
	{

		for (Entity* entity : _s_scripts)
		{
			CGX_VERIFY(entity);

			for (const auto& component: entity->GetScripts())
			{
				Script* script = static_cast<Script*>(component.second.get());

				if (script->IsEnabled())
					script->Update(entity);
			}
		}
	}

	void ScriptSystem::_DoScriptWarmUp(Entity* entity)
	{
		const std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Interal::Component>>& scripts = entity->GetScripts();

		for (const auto& scriptPtr : scripts)
		{
			Script* script = static_cast<Script*>(scriptPtr.second.get());
			if (script)
			{
				script->Awake(entity);
			}
		}

		for (const auto& scriptPtr : scripts)
		{
			Script* script = static_cast<Script*>(scriptPtr.second.get());
			if (script)
			{
				script->Start(entity);
			}
		}
	}

	void ScriptSystem::WarmUp()
	{
		for (Entity* entity : _s_scripts)
		{
			ScriptSystem::_DoScriptWarmUp(entity);
		}
	}

	void ScriptSystem::RegisterScript(Entity* entity)
	{
		CGX_VERIFY(entity);

		if (entity->GetScripts().size() > 0)
			_s_scripts.insert(entity);
	}

	void ScriptSystem::RunTimeRegisterScript(Entity* entity)
	{
		CGX_VERIFY(entity);

		if (entity->GetScripts().size() > 0)
		{
			RegisterScript(entity);
			ScriptSystem::_DoScriptWarmUp(entity);
		}
	}

	void ScriptSystem::DeregisterScript(Entity* entity)
	{
		CGX_VERIFY(entity);

		auto itEntity = _s_scripts.find(entity);

		if (itEntity == _s_scripts.end())
			return;

		Entity* entityToRemove = *itEntity;

		if (entityToRemove->GetScripts().size() == 1)
			_s_scripts.erase(itEntity);
	}
}