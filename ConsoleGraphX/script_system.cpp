#include "script_system.h"

std::unordered_map<Entity*, std::vector<Script*>> ScriptSystem::_s_scripts;
std::vector<std::pair<Entity*, Script*>> ScriptSystem::_s_scriptsToDelete;


void ScriptSystem::Initialize() const 
{
	Dispatcher<Entity*>::RegisterListener("AddComponentStruct script", RegisterScript);
	// need to add support for removing single scripts
	Dispatcher<Entity*>::RegisterListener("RemoveComponentStruct script", DeregisterScripts);
	Dispatcher<Entity*>::RegisterListener("RunTimeScriptAddition", RunTimeRegisterScript);
}

void ScriptSystem::Update(float delta_time) const
{
	ScriptSystem::DeleteScripts();

	for (std::pair<Entity*, std::vector<Script*>> scriptPair : _s_scripts)
	{
		if (scriptPair.first == nullptr)
		{
			Debugger::S_LogMessage("[SCRIPT SYSTEM] update, null entity pointer");
			DeregisterScripts(scriptPair.first);
		}

		for (auto scriptIt = scriptPair.second.rbegin(); scriptIt != scriptPair.second.rend(); ++scriptIt)
		{
			Script* script = *scriptIt;

			if (script == nullptr)
			{
				Debugger::S_LogMessage("[SCRIPT SYSTEM] update, null script pointer");
				DeregisterScriptWS(scriptPair.first, script);
			}

			if (script->IsEnabled())
			{
				script->Update(scriptPair.first);
			}
		}
	}
}

void ScriptSystem::WarmUp()
{
	for (std::pair<Entity*, std::vector<Script*>> scriptPair : _s_scripts)
	{
		for (Script* script : scriptPair.second)
		{
			script->Awake(scriptPair.first);
		}

		for (Script* script : scriptPair.second)
		{
			script->Start(scriptPair.first);
		}
	}
}

void ScriptSystem::RegisterScript(Entity* entity)
{
	if (entity == nullptr)
		throw std::runtime_error("[SCRIPT SYSTEM] reg, null entity pointer");

	Script* script = (Script*)(entity->GetComponentByID(ComponentID::script));
	if (!script)
		return;  // No script component found, nothing to register

	auto& scriptVector = _s_scripts[entity];
	scriptVector.push_back(script);
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

		if (!script->IsEnabled())
			return;

		script->Awake(entity);
		script->Start(entity);
	}
}

void ScriptSystem::DeregisterScripts(Entity* entity)
{
	if (entity == nullptr)
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] dereg, null entity pointer");
	}

	auto itScript = _s_scripts.find(entity);

	if (itScript == _s_scripts.end())
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] deregs, unable to remove scripts due to no entity found");
		return;
	}

	for (auto it =  itScript->second.rbegin(); it != itScript->second.rend(); ++it) {
		DeregisterScriptWS(entity, *it);
	}
}


// DeRegister script using script pointer (With Script, WS)
void ScriptSystem::DeregisterScriptWS(Entity* entity, Script* script)
{
	// we have already checked for null entity in prev function call
	// if we made it this fair the entity is safe
	if (script == nullptr)
	{
		Debugger::S_LogMessage("[SCRIPT SYSTEM] dereg script, null script pointer");
		return;
	}
		
	_s_scriptsToDelete.push_back(std::make_pair(entity, script));
}

void ScriptSystem::DeleteScripts()
{
	for (auto entityScriptPair = _s_scriptsToDelete.rbegin(); entityScriptPair != _s_scriptsToDelete.rend(); entityScriptPair++)
	{
		Entity* entity = entityScriptPair->first;
		Script* script = entityScriptPair->second;

		auto itEntityScriptPair = _s_scripts.find(entity);

		if (itEntityScriptPair == _s_scripts.end())
			throw std::runtime_error("[SCRIPT SYSTEM] delete script, could not find entity script pair for entity: " + std::to_string(entity->m_id));

		auto itScript = std::find(itEntityScriptPair->second.begin(), itEntityScriptPair->second.end(), script);

		if (itScript == itEntityScriptPair->second.end())
			throw std::runtime_error("[SCRIPT SYSTEM] delete script, could not find script for entity: " + std::to_string(entity->m_id));

		itEntityScriptPair->second.erase(itScript);

		if(itEntityScriptPair->second.size() == 0)
		{
			_s_scripts.erase(itEntityScriptPair);
		}
	}

	_s_scriptsToDelete.clear();
}

