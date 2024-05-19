#pragma once
#include "camera.h"
#include <unordered_set>
#include "base_system.h"
#include "entity.h"

namespace ConsoleGraphX
{
	class CameraSystem : public BaseSystem
	{
	private:
		static std::unordered_set<Camera*> _s_backupCamera;
		static std::unordered_set<Camera*> _s_cameras;
		static std::unordered_set<Camera*> _s_activeCameras;

		static void _RemoveCameraFromActiveSet(Camera* cam);
		static void _DeregisterCameraAttachedToEntity(Entity* entity);
		static void _AddCameraToActive(Camera* cam);


	public:
		void Initialize() override;
		void Update(float delta_time) override;

		static void RegisterCamera(Camera* cam);
		static void DeregisterCamera(Camera* cam);

		static void SetCameraState(Camera* cam, bool state);

		const static std::unordered_set<Camera*> GetActiveCameras();

	};


};

