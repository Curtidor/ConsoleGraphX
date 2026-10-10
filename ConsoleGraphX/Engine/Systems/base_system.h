#pragma once
namespace ConsoleGraphX_Internal
{
	class FrameExecutor;
};

namespace ConsoleGraphX
{
	class SceneSystem;
}
namespace ConsoleGraphX
{
	
	class BaseSystem
	{
		friend class ConsoleGraphX_Internal::FrameExecutor; // allow SystemManager to call EndOfFrame without exposing it publicly

	protected:
		virtual void EndOfFrame() {};

	public:
		virtual void Initialize() = 0;
		virtual void Update(float delta_time, SceneSystem& sceneSystem) = 0;
		virtual void ShutDown() = 0;

		virtual ~BaseSystem() = default;

	};

};
