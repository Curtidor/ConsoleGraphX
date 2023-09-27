#pragma once
#include <unordered_set>
#include "system.h"
#include "script.h"

class ScriptSystem : public System 
{
private:
	static std::unordered_set<Script*> _m_scripts;

public:
	void Initalize() const override;
	void Update() const override;

	static void WarmUp();
	static void StartScript(Script* script);

	static void RegisterScript(Entity* entity_w_script);
	static void RunTimeRegisterScript(Entity* entity_w_script);
	static void DeregisterScript(Entity* entity_w_script);
	
};