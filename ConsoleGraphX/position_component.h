#pragma once
#include "component.h"
#include "transform.h"

namespace ConsoleGraphX_Internal
{
	struct PositionComponentBase : public Component
	{
		ConsoleGraphX::Transform m_transform;

		PositionComponentBase();
		PositionComponentBase(const ConsoleGraphX::Transform& other);
	};
};