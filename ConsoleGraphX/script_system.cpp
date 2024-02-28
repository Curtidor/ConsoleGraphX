#include "script_system.h"

std::unordered_map<Entity*, std::unordered_set<Script*>> ScriptSystem::_m_scripts;

void ScriptSystem::Initialize() const 
{
	Dispatcher<Entity*>::RegisterListener("AddComponentStruct script", RegisterScript);
	// need to add support for removing single scripts
	Dispatcher<Entity*>::RegisterListener("RemoveComponentStruct script", DeregisterScripts);

	Dispatcher<Entity*>::RegisterListener("RunTimeAddScript", RunTimeRegisterScript);
	// need to add support for removing single scripts
	Dispatcher<Entity*>::RegisterListener("RemoveScript", DeregisterScripts);
}

void ScriptSystem::Update(float delta_time) const
{
	for (std::pair<Entity*, std::unordered_set<Script*>> scriptPair : _m_scripts)
	{
		if (scriptPair.first == nullptr)
		{
			Debugger::S_LogMessage("[SCRIPT SYSTEM] update, null entity pointer");
			DeregisterScripts(scriptPair.first);
		}

		for (Script* script : scriptPair.second)
		{
			if (script == nullptr)
			{
				Debugger::S_LogMessage("[SCRIPT SYSTEM] update, null script pointer");
				DeregisterScriptWS(scriptPair.first, script);
			}

			script->Update(scriptPair.first);
		}
	}
}

void ScriptSystem::WarmUp()
{
	for (std::pair<Entity*, std::unordered_set<Script*>> scriptPair : _m_scripts)
	{
		for (Script* script : scriptPair.second)
		{
			StartScript(scriptPair.first, script);
		}
	}
}

void ScriptSystem::StartScript(Entity* entity, Script* script)
{
	script->Awake(entity);
	script->Start(entity);

	if (!script->IsEnabled())
	{
		DeregisterScriptWS(entity, script);
	}
}

void ScriptSystem::RegisterScript(Entity* entity)
{
	if (entity == nullptr)
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] reg, null entity pointer");
		return;
	}

	if( Script* script = (Script*)entity->GetComponentByID(ComponentID::script))
	{
		auto it = ScriptSystem::_m_scripts.find(entity);

		if (it != _m_scripts.end())
		{
			it->second.insert(script);
		} 
		else
		{
			std::unordered_set<Script*> scriptSet;
			scriptSet.insert(script);

			_m_scripts.insert(std::make_pair(entity, scriptSet));
		}
	}
}

void ScriptSystem::RunTimeRegisterScript(Entity* entity)
{
	if (entity == nullptr)
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] reg, null entity pointer");
		return;
	}

	if (Script* script = (Script*)entity->GetComponentByID(ComponentID::script))
	{
		RegisterScript(entity);
		StartScript(entity, script);
	}
}

void ScriptSystem::DeregisterScripts(Entity* entity)
{
	if (entity == nullptr)
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] dereg, null entity pointer");
	}

	auto it = _m_scripts.find(entity);

	if (it == _m_scripts.end())
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] deregs, unable to remove scripts due to no entity found");
		return;
	}

	std::vector<Script*> scriptsToRemove(it->second.begin(), it->second.end());
	for (auto it = scriptsToRemove.rbegin(); it != scriptsToRemove.rend(); ++it) {
		DeregisterScriptWS(entity, *it);
	}
}


// DeRegister script using script pointer (With Script, WS)
void ScriptSystem::DeregisterScriptWS(Entity* entity, Script* script)
{
	// we have already checked for null entity in prev function call
	// if we made it this fair the entity is safe
	auto entityScriptPair = _m_scripts.find(entity);

	if (script == nullptr)
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] dereg script, null script pointer");
	}
		
	if (entityScriptPair == _m_scripts.end())
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] dereg script, no script found!");
		return;
	}
	
	auto itScript = entityScriptPair->second.find(script);
	if (itScript != entityScriptPair->second.end())
	{
		entityScriptPair->second.erase(itScript);
	}

	if (entityScriptPair->second.size() == 0)
	{
		_m_scripts.erase(entity);
	}
}

