#pragma once
#include "camera.h"
#include <unordered_set>
#include "system.h"
#include "entity.h"

namespace ConsoleGraphX
{
	class CameraSystem : public System
	{
	private:
		static std::unordered_set<Camera*> _s_backupCamera;
		static std::unordered_set<Camera*> _s_cameras;
		static std::unordered_set<Camera*> _s_activeCameras;

		static void _RemoveCameraFromActive(Camera* cam);
		static void _CameraRemovedHandler(Entity* entity);
		static void _AddCameraToActive(Camera* cam);


	public:
		void Initialize() const override;
		void Update(float delta_time) const override;

		static void RegisterCamera(Camera* cam);
		static void DeregisterCamera(Camera* cam);

		static void SetCameraState(Camera* cam, bool state);

		const static std::unordered_set<Camera*> GetActiveCameras();

	};


};

