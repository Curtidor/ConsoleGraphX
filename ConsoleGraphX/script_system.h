#pragma once
#include <unordered_set>
#include "system.h"
#include "script.h"

class ScriptSystem : public System 
{
private:
	static std::unordered_set<Script*> _m_scripts;

public:
	void Initialize() const override;
	void Update(float delta_time) const override;

	static void WarmUp();

	static void RegisterScript(Entity* entity);
	static void RunTimeRegisterScript(Entity* entity);
	static void DeregisterScript(Entity* entity);

	static void StartScript(Script* script);
	static void DeregisterScriptWS(Script* script);
	
};