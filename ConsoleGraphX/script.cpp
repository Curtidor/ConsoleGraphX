#include "script.h"
#include "component.h"
#include "entity.h"

Script::Script(): _m_isEnabled(true)
{}

Script::Script(const Script& script) : _m_isEnabled(true)
{}

void Script::Awake(Entity* owner) {}
void Script::Start(Entity* owner) {}
void Script::Update(Entity* owner) {}

bool Script::IsEnabled()
{
	return this->_m_isEnabled;
}

void Script::SetState(bool state)
{
	this->_m_isEnabled = state;
}

int Script::GetID() const 
{
	return ComponentID::script;
}

Component* Script::Clone() const 
{
	Script* clone = new Script(*this);

	return clone;
}

