#include "script.h"

Script::Script(): m_is_enabled(false)
{}

void Script::Awake(Entity* owner) {}
void Script::Start(Entity* owner) {}
void Script::Update(Entity* owner) {}

bool Script::IsEnabled()
{
	return this->m_is_enabled;
}

int Script::GetID() const 
{
	return ComponentID::script;
}

