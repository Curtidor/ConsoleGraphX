#include "script.h"

Script::Script(Entity* entity)
	: m_entity(entity) 
{}

void Script::Awake() {}
void Script::Start() {}
bool Script::Update() { return false; }

int Script::GetID() const 
{
	return ComponentID::script;
}

