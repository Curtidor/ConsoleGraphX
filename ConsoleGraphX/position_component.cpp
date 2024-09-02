#include "position_component.h"
#include "transform.h"

namespace ConsoleGraphX_Internal
{
	PositionComponentBase::PositionComponentBase() : m_transform(ConsoleGraphX::Transform())
	{}

	PositionComponentBase::PositionComponentBase(const ConsoleGraphX::Transform& other) : m_transform(other)
	{}
};