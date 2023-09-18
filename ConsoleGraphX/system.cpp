#include "system.h"

std::unordered_set<System*> System::_s_systems;

System::~System()
{
	for (System* s : _s_systems)
	{
		delete s;
	}
}
void System::Initalize() const {}

void System::Update() const 
{
	for (System* s : _s_systems)
	{
		s->Update();
	}
}
