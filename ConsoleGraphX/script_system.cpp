#include "script_system.h"

std::unordered_set<Script*> ScriptSystem::_m_scripts;

void ScriptSystem::Initalize() const 
{
	Dispatcher<Entity*>::RegisterListener("AddComponentstruct Script", RegisterScript);
	Dispatcher<Entity*>::RegisterListener("RemoveComponentstruct Script", DeregisterScript);
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
		script->Start();
	}

	for (Script* script : _m_scripts)
	{
		// if the script has no update remove it from the script set as we are done with it
		if(!script->Update())
		{
			auto it = _m_scripts.find(script);

			_m_scripts.erase(it);
		}
	}
}

void ScriptSystem::RegisterScript(Entity* entity)
{
	if(Script* script = (Script*)entity->GetComponentByID(ComponentID::script))
	{
		script->Awake();

		_m_scripts.insert(script);
	}
}

void ScriptSystem::DeregisterScript(Entity* entity)
{
	if (Script* script = entity->GetComponent<Script>())
	{
		auto it = _m_scripts.find(script);

		_m_scripts.erase(it);
	}
}

