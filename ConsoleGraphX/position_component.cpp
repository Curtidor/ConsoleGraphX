#include <limits>
#include "position_component.h"
#include "transform.h"
#include "resourcec_manager.h"

namespace ConsoleGraphX_Internal
{
	PositionComponentBase::PositionComponentBase()
	{
		m_transformID = (std::numeric_limits<ConsoleGraphX::TransformID>::max)();
	}

	PositionComponentBase::PositionComponentBase(const ConsoleGraphX::TransformID transformID)
	{
		m_transformID = transformID;
	}

	ConsoleGraphX::Transform* PositionComponentBase::GetTransform() const
	{
		return ResourceManager::Instance().GetResource<ConsoleGraphX::Transform>(m_transformID);
	}
};