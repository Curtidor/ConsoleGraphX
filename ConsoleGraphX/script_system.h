#pragma once
#include <unordered_map>
#include "system.h"
#include "script.h"

class ScriptSystem : public System 
{
private:
	static std::unordered_map<Entity*, std::unordered_set<Script*>> _m_scripts;

	static void DeregisterScriptWS(Entity* entity, Script* script);

public:
	void Initialize() const override;
	void Update(float delta_time) const override;

	static void WarmUp();

	static void RegisterScript(Entity* entity);
	static void RunTimeRegisterScript(Entity* entity);

	static void DeregisterScripts(Entity* entity);
	static void StartScript(Entity* entity, Script* script);
};