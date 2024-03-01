#include "component.h"

/**
 * @brief Default constructor for the base Component class.
 */
Component::Component()
{}

Component::~Component()
{}

int Component::GetID() const 
{
	return ComponentID::base;
}

const std::string Component::ComponentName() const
{
	return typeid(*this).name();
}