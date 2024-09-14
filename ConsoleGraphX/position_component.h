#pragma once
#include "component.h"
#include "transform.h"

namespace ConsoleGraphX_Internal
{
	struct PositionComponentBase
	{
		ConsoleGraphX::TransformID m_transformID;

		PositionComponentBase();
		PositionComponentBase(const ConsoleGraphX::Transform& other);
		PositionComponentBase(const ConsoleGraphX::TransformID other);

		ConsoleGraphX::Transform* GetTransform() const;
	};
};