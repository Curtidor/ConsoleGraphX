#include <limits>
#include "position_component.h"
#include "transform.h"
#include "resource_manager.h"


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
		return ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager().GetResource<ConsoleGraphX::Transform>(m_transformID);
	}
};