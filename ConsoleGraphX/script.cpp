#include "script.h"

Script::Script(Entity* entity)
	: m_entity(entity), m_is_enabled(false)
{}

void Script::Awake() {}
void Script::Start() {}
void Script::Update() {}

bool Script::IsEnabled()
{
	return this->m_is_enabled;
}

void Script::SetStatus(bool state)
{
	this->m_is_enabled = state;

	std::string event_name = state == true ? "RumTimeAddScript" : "RemoveScript";
	
	// notify the script system that a script has been added or removed based on the state
	Dispatcher<Entity*>::Notify(event_name, this->m_entity);
}


int Script::GetID() const 
{
	return ComponentID::script;
}

