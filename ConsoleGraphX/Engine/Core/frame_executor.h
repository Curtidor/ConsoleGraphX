#pragma once
#include <cstdint>
#include "Engine/Systems/base_system.h"

namespace ConsoleGraphX_Internal
{
	enum class FrameExecutionStage : uint8_t
	{
		START_OF_FRAME,
		END_OF_FRAME
	};

	class FrameExecutor
	{
	public:
		void ExecuteFrameStage(FrameExecutionStage stage, ConsoleGraphX::BaseSystem& system)
		{
			switch (stage)
			{
			case FrameExecutionStage::END_OF_FRAME:
				system.EndOfFrame();
				break;
			default:
				break;
			}
		}
	};
};