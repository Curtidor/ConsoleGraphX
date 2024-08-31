#pragma once
#include <unordered_set>
#include "base_system.h"
#include "entity.h"

namespace ConsoleGraphX
{
	class ScriptSystem : public BaseSystem
	{
	private:
		static std::unordered_set<Entity*> _m_scripted_entities;

		static void _DoScriptWarmUp(Entity*);


	public:
		void Initialize() override;
		void Update(float delta_time) override;

		static void WarmUp();

		static void RegisterScript(Entity* entity);
		static void RunTimeRegisterScript(Entity* entity);

		static void DeregisterScript(Entity* entity);
	};
};

