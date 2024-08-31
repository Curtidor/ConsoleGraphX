#include <string>
#include <unordered_set>
#include <unordered_map>
#include "dispatcher.h"
#include "entity.h"
#include "script.h"
#include "script_system.h"
#include "verify_macro.h"
#include "base_component_pool.h"
#include "component_id.h"
#include "component_manager.h"
#include "component_pool.h"

namespace ConsoleGraphX
{
	std::unordered_set<Entity*> ScriptSystem::_m_scripted_entities;

	// TODO when script state changes we should stop calling update on that script or start calling update deping on the state

	void ScriptSystem::Initialize() 
	{
		std::string objectName = typeid(Script).name();

		ConsoleGraphX_Internal::Dispatcher<Entity*>::RegisterListener("AddComponent" + objectName, RegisterScript);
		// TODO need to add support for removing single scripts
		ConsoleGraphX_Internal::Dispatcher<Entity*>::RegisterListener("RemoveComponent" + objectName, DeregisterScript);
		ConsoleGraphX_Internal::Dispatcher<Entity*>::RegisterListener("RunTimeScriptAddition" + objectName, RunTimeRegisterScript);
	}

	void ScriptSystem::Update(float delta_time)
	{
		std::unordered_map<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex> entityScriptIndexes;
		ConsoleGraphX_Internal::ComponentPool<Script>* scriptPool = ConsoleGraphX_Internal::ComponentManager::Instance().GetComponentPool<Script>();

		for (auto it = _m_scripted_entities.begin(); it != _m_scripted_entities.end();)
		{
			Entity* entity = *it;
			entityScriptIndexes = entity->GetScripts();

			if (entityScriptIndexes.empty())
			{
				it = _m_scripted_entities.erase(it);
				continue;
			}

			for (const auto& idIndexPair : entityScriptIndexes)
			{
				scriptPool->GetComponentFromPool(idIndexPair.second)->Update(entity);
			}
			++it;
		}

	}

	void ScriptSystem::_DoScriptWarmUp(Entity* entity)
	{

		for (const auto& scriptPair : entity->GetScripts())
		{
			Script* script = ConsoleGraphX_Internal::ComponentManager::Instance().GetComponentPool<Script>()->GetComponentFromPool(scriptPair.second);
			
			if (script)
			{
				script->Awake(entity);
			}
		}

		for (const auto& scriptPair : entity->GetScripts())
		{
			Script* script = ConsoleGraphX_Internal::ComponentManager::Instance().GetComponentPool<Script>()->GetComponentFromPool(scriptPair.second);

			if (script)
			{
				script->Start(entity);
			}
		}
	}

	void ScriptSystem::WarmUp()
	{
		for (Entity* entity : _m_scripted_entities)
		{
			ScriptSystem::_DoScriptWarmUp(entity);
		}
	}

	void ScriptSystem::RegisterScript(Entity* entity)
	{
		CGX_VERIFY(entity);

		if (entity->GetScripts().size() > 0)
			_m_scripted_entities.insert(entity);
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

		auto itEntity = _m_scripted_entities.find(entity);

		if (itEntity == _m_scripted_entities.end())
			return;

		if ((*itEntity)->GetScripts().size() == 1)
			_m_scripted_entities.erase(itEntity);
	}
}