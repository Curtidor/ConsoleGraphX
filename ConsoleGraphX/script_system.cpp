#include <stdexcept>
#include <string>
#include <unordered_set>
#include "component.h"
#include "debugger.h"
#include "dispatcher.h"
#include "entity.h"
#include "script.h"
#include "script_system.h"

namespace ConsoleGraphX
{
	std::unordered_set<ConsoleGraphX::Entity*> ScriptSystem::_s_scripts;

	// TODO when script state changes we should stop calling update on that script or start calling update deping on the state

	void ScriptSystem::Initialize() const
	{
		std::string objectName = typeid(Script).name();

		//AddComponentstruct ConsoleGraphX::Script
		ConsoleGraphX_Interal::Dispatcher<ConsoleGraphX::Entity*>::RegisterListener("AddComponent" + objectName, RegisterScript);
		// need to add support for removing single scripts
		ConsoleGraphX_Interal::Dispatcher<ConsoleGraphX::Entity*>::RegisterListener("RemoveComponent" + objectName, DeregisterScript);
		ConsoleGraphX_Interal::Dispatcher<ConsoleGraphX::Entity*>::RegisterListener("RunTimeScriptAddition" + objectName, RunTimeRegisterScript);
	}

	void ScriptSystem::Update(float delta_time) const
	{

		for (ConsoleGraphX::Entity* entity : _s_scripts)
		{
			if (entity == nullptr)
				throw std::runtime_error("[SCRIPT SYSTEM] update, null entity pointer");

			for (ConsoleGraphX_Interal::Component* component : entity->GetScripts())
			{
				ConsoleGraphX::Script* script = static_cast<ConsoleGraphX::Script*>(component);

				if (script->IsEnabled())
					script->Update(entity);
			}

		}
	}

	void ScriptSystem::_DoScriptWarmUp(ConsoleGraphX::Entity* entity)
	{
		std::unordered_set<ConsoleGraphX_Interal::Component*> scripts = entity->GetScripts();

		for (ConsoleGraphX_Interal::Component* component : scripts)
		{
			ConsoleGraphX::Script* script = static_cast<ConsoleGraphX::Script*>(component);

			script->Awake(entity);
		}

		for (ConsoleGraphX_Interal::Component* component : scripts)
		{
			ConsoleGraphX::Script* script = static_cast<ConsoleGraphX::Script*>(component);

			script->Start(entity);
		}
	}

	void ScriptSystem::WarmUp()
	{
		for (ConsoleGraphX::Entity* entity : _s_scripts)
		{
			ScriptSystem::_DoScriptWarmUp(entity);
		}
	}

	void ScriptSystem::RegisterScript(ConsoleGraphX::Entity* entity)
	{
		if (entity == nullptr)
			throw std::runtime_error("[SCRIPT SYSTEM] reg, null entity pointer");

		if (entity->GetScripts().size() > 0)
			_s_scripts.insert(entity);
	}


	void ScriptSystem::RunTimeRegisterScript(ConsoleGraphX::Entity* entity)
	{
		if (entity == nullptr)
		{
			ConsoleGraphX_Interal::Debugger::S_LogMessage("[SCRIPT SYSTEM] reg, null entity pointer");
			return;
		}

		if (entity->GetScripts().size() > 0)
		{
			RegisterScript(entity);
			ScriptSystem::_DoScriptWarmUp(entity);
		}
	}

	void ScriptSystem::DeregisterScript(Entity* entity)
	{
		if (entity == nullptr)
			throw std::runtime_error("[SCRIPT SYSTEM] dereg, null entity pointer");

		auto itEntity = _s_scripts.find(entity);

		if (itEntity == _s_scripts.end())
			return;

		Entity* entityToRemove = *itEntity;

		if (entityToRemove->GetScripts().size() == 1)
			_s_scripts.erase(itEntity);
	}
}

