#include "entity.h"
#include "script.h"
#include "script_system.h"
#include "verify_macro.h"
#include "component_manager.h"
#include "component_pool.h"
#include "scene_system.h"
#include "scene.h"
#include "dispatcher.h"

namespace ConsoleGraphX
{

	void ScriptSystem::Initialize() 
	{
		ConsoleGraphX_Internal::Dispatcher<EntityID>::RegisterListener("AddScript", RegisterScriptedEntity);
		ConsoleGraphX_Internal::Dispatcher<EntityID>::RegisterListener("AddScriptRunTime", RunTimeRegisterScript);
		ConsoleGraphX_Internal::Dispatcher<EntityID>::RegisterListener("RemoveScript", DeregisterScript);
	}

	void ScriptSystem::Update(float delta_time)
	{
		ConsoleGraphX_Internal::ComponentPool<Script>* scriptPool = ConsoleGraphX_Internal::ComponentManager::Instance().GetComponentPool<Script>();
		
		Scene* activeScene = SceneSystem::GetActiveScene();
		for (auto it = _m_scripted_entities.begin(); it != _m_scripted_entities.end();)
		{
			EntityID entityId = *it;
			Entity* entity = activeScene->GetEntity(entityId);

			const auto& entityScriptIndexes = entity->GetScripts();

			for (const auto& idIndexPair : entityScriptIndexes)
			{
				Script* script = scriptPool->GetComponentFromPool(idIndexPair.second);

				if (script->IsEnabled())
				{
					script->Update(entity);
				}
			}
			++it;
		}
	}

	void ScriptSystem::_ScriptWarmUpImpl(EntityID entityId)
	{
		Entity* entity = SceneSystem::GetActiveScene()->GetEntity(entityId);

		CGX_VERIFY(entity);

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

	void ScriptSystem::ScriptWarmUp()
	{
		for (EntityID entityId : _m_scripted_entities)
		{
			ScriptSystem::_ScriptWarmUpImpl(entityId);
		}
	}

	void ScriptSystem::RegisterScriptedEntity(EntityID entityId)
	{
		_m_scripted_entities.insert(entityId);
	}

	void ScriptSystem::RunTimeRegisterScript(EntityID entityId)
	{
		RegisterScriptedEntity(entityId);
		ScriptSystem::_ScriptWarmUpImpl(entityId);
	}

	void ScriptSystem::DeregisterScript(EntityID entityId)
	{
		auto itEntityId = _m_scripted_entities.find(entityId);

		if (itEntityId == _m_scripted_entities.end())
			return;

		Entity* entity = SceneSystem::GetActiveScene()->GetEntity(entityId);
		
		CGX_VERIFY(entity);

		if (entity->GetScripts().size() == 0)
			_m_scripted_entities.erase(itEntityId);
	}
}