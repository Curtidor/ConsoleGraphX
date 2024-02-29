#include "spawner.h"

Spawner::Spawner() : _m_isEnabled(false)
{}

void Spawner::SetEnableState(bool state)
{
	this->_m_isEnabled = state;
}

void Spawner::Spawn()
{
	Dispatcher<Spawner*>::Notify("SpawnedEntity", this);
}

int Spawner::GetID() const
{
	return ComponentID::spawner;
}

Component* Spawner::Clone() const 
{
	Spawner* clone = new Spawner();

	return clone;
}