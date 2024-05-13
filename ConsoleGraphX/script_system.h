#pragma once
#include <unordered_set>
#include "system.h"
#include "entity.h"

namespace ConsoleGraphX
{
	class ScriptSystem : public System
	{
	private:
		static std::unordered_set<Entity*> _s_scripts;

		static void _DoScriptWarmUp(Entity*);


	public:
		void Initialize() const override;
		void Update(float delta_time) const override;

		static void WarmUp();

		static void RegisterScript(Entity* entity);
		static void RunTimeRegisterScript(Entity* entity);

		static void DeregisterScript(Entity* entity);
	};
};

