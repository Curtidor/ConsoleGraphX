#include "script_system.h"
#include <stdexcept>
#include <unordered_set>
#include "component.h"
#include "debugger.h"
#include "dispatcher.h"
#include "entity.h"
#include "script.h"

std::unordered_set<Entity*> ScriptSystem::_s_scripts;

// TODO when script state changes we should stop calling update on that script or start calling update deping on the state

void ScriptSystem::Initialize() const 
{
	Dispatcher<Entity*>::RegisterListener("AddComponentstruct Script", RegisterScript);
	// need to add support for removing single scripts
	Dispatcher<Entity*>::RegisterListener("RemoveComponentstruct Script", DeregisterScript);
	Dispatcher<Entity*>::RegisterListener("RunTimeScriptAddition", RunTimeRegisterScript);
}

void ScriptSystem::Update(float delta_time) const
{

	for (Entity* entity : _s_scripts)
	{
		if (entity == nullptr)
			throw std::runtime_error("[SCRIPT SYSTEM] update, null entity pointer");

		for (Component* component : entity->GetScripts())
		{
			Script* script = static_cast<Script*>(component);

			if (script->IsEnabled())
				script->Update(entity);
		}

	}
}

void ScriptSystem::_DoScriptWarmUp(Entity* entity)
{
	std::unordered_set<Component*> scripts = entity->GetScripts();

	for (Component* component : scripts)
	{
		Script* script = static_cast<Script*>(component);

		script->Awake(entity);
	}

	for (Component* component : scripts)
	{
		Script* script = static_cast<Script*>(component);

		script->Start(entity);
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
	if (entity == nullptr)
		throw std::runtime_error("[SCRIPT SYSTEM] reg, null entity pointer");

	if (entity->GetScripts().size() > 0)
		_s_scripts.insert(entity);
}

 
void ScriptSystem::RunTimeRegisterScript(Entity* entity)
{
	if (entity == nullptr)
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] reg, null entity pointer");
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

