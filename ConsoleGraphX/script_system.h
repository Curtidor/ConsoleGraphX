#pragma once
#include <unordered_set>
#include "base_system.h"

namespace ConsoleGraphX
{
	class ScriptSystem : public BaseSystem
	{
	private:
		using EntityID = size_t;
		static inline std::unordered_set<EntityID> _m_scripted_entities;

	private:
		static void _ScriptWarmUpImpl(EntityID entityId);

	public:
		void Initialize() override;
		void Update(float delta_time) override;

		static void ScriptWarmUp();

		static void RegisterScriptedEntity(EntityID entityId);
		static void RunTimeRegisterScript(EntityID entityId);

		static void DeregisterScript(EntityID entityId);
	};
};

