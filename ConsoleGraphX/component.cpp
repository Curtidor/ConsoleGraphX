#include "component.h"
#include <typeindex>
#include <string>

/**
 * @brief Default constructor for the base Component class.
 */
namespace ConsoleGraphX_Internal
{
	Component::Component()
	{}

	Component::~Component()
	{}

	void Component::Clone(Component* component) const
	{
		*component = *this;
	}


	const std::string Component::ComponentName() const
	{
		return typeid(*this).name();
	}
};

