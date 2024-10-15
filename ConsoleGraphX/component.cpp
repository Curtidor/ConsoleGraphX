#include "CGXPCH.h"
#include "component.h"

/**
 * @brief Default constructor for the base Component class.
 */
namespace ConsoleGraphX_Internal
{
	Component::Component()
	{}

	void Component::Clone(Component* component) const
	{
		*component = *this;
	}
};

