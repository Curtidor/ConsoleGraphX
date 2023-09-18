#pragma once
#include "component_system.h"

// Define the static member variable _registered_components here
std::unordered_set<std::string> ComponentSystem::_registered_components;

// Implement the IsComponentRegistered function
bool ComponentSystem::IsComponentRegistered(const std::string& typeName)
{
    return (_registered_components.find(typeName) != _registered_components.end());
}
