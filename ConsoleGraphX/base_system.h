#pragma once

namespace ConsoleGraphX
{
	class BaseSystem
	{
	public:
		virtual void Initialize() = 0;
		virtual void Update(float delta_time) = 0;

		virtual ~BaseSystem() = default;
	};
};
