#include "script_system.h"

std::unordered_set<Script*> ScriptSystem::_m_scripts;

void ScriptSystem::Initalize() const 
{
	Dispatcher<Entity*>::RegisterListener("AddScript", RegisterScript);
	Dispatcher<Entity*>::RegisterListener("RunTimeAddScript", RunTimeRegisterScript);

	Dispatcher<Entity*>::RegisterListener("RemoveScript", DeregisterScript);
}

void ScriptSystem::Update() const
{
	for (Script* entity : _m_scripts)
	{
		entity->Update();
	}
}

void ScriptSystem::WarmUp()
{
	for (Script* script : _m_scripts)
	{
		StartScript(script);
	}
}

void ScriptSystem::StartScript(Script* script)
{
	script->Awake();
	script->Start();

	if (!script->IsEnabled())
	{
		auto it = _m_scripts.find(script);

		_m_scripts.erase(it);
	}
}

void ScriptSystem::RegisterScript(Entity* entity)
{
	if(Script* script = (Script*)entity->GetComponentByID(ComponentID::script))
	{
		_m_scripts.insert(script);
	}
}

void ScriptSystem::RunTimeRegisterScript(Entity* entity)
{
	if (Script* script = (Script*)entity->GetComponentByID(ComponentID::script))
	{
		_m_scripts.insert(script);

		StartScript(script);
	}
}

void ScriptSystem::DeregisterScript(Entity* entity)
{
	if (Script* script = (Script*)entity->GetComponentByID(ComponentID::script))
	{
		auto it = _m_scripts.find(script);

		_m_scripts.erase(it);
	}
}

