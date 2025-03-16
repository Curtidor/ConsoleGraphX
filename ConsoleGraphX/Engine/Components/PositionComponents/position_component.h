#pragma once
#include "Engine\Components\transform.h"

namespace ConsoleGraphX_Internal
{
	struct PositionComponentBase
	{
		ConsoleGraphX::TransformID m_transformID;

		PositionComponentBase();
		PositionComponentBase(const ConsoleGraphX::TransformID other);

		ConsoleGraphX::Transform* GetTransform() const;
	};
};