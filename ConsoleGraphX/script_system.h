#pragma once
#include <unordered_map>
#include "system.h"
#include "script.h"

class ScriptSystem : public System 
{
private:
	static std::unordered_map<Entity*, std::vector<Script*>> _s_scripts;
	static std::vector<std::pair<Entity*, Script*>> _s_scriptsToDelete;

	static void DeregisterScriptWS(Entity* entity, Script* script);
	static void DeleteScripts();

public:
	void Initialize() const override;
	void Update(float delta_time) const override;

	static void WarmUp();

	static void RegisterScript(Entity* entity);
	static void RunTimeRegisterScript(Entity* entity);

	static void DeregisterScripts(Entity* entity);
};