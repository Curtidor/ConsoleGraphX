#pragma once
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <string>
#include <typeinfo>
#include "component.h"

class ComponentSystem
{
private:
    static std::unordered_set<std::string> _registered_components;

public:
    template <typename ComponentType>
    static void RegisterComponent()
    {
        static_assert(std::is_base_of<Component, ComponentType>::value, "The passed type must be derived from Component.");

        // Get the type name as a string
        std::string typeName = typeid(ComponentType).name();

        if (!std::is_default_constructible<ComponentType>::value)
        {
            std::cerr << "Error: The passed type must have a default constructor: " << typeName << std::endl;
            return; // Skip registration or handle the error as needed
        }

        // Check if the component type is already registered
        if (ComponentSystem::IsComponentRegistered(typeName))
        {
            std::cerr << "Error: This component type has already been registered: " << typeName << std::endl;
            return; // Skip registration 
        }

        ComponentSystem::_registered_components.insert(typeName);
    }

    static bool IsComponentRegistered(const std::string& typeName);
};
