#include <unordered_set>
#include <string>
#include "camera_system.h"
#include "camera.h"
#include "debugger.h"
#include "dispatcher.h"
#include "entity.h"
#include "screen.h"
#include "vector3.h"
#include "verify_macro.h"

namespace ConsoleGraphX
{
	std::unordered_set<Camera*> CameraSystem::_s_backupCamera;
	std::unordered_set<Camera*> CameraSystem::_s_cameras;
	std::unordered_set<Camera*> CameraSystem::_s_activeCameras;

	void CameraSystem::Initialize()
	{
		std::string objectName = typeid(Camera).name();

		ConsoleGraphX_Internal::Dispatcher<Entity*>::RegisterListener("RemoveComponent" + objectName, _DeregisterCameraAttachedToEntity);
		Camera* backupCam = new Camera(ConsoleGraphX_Internal::Screen::GetWidth_A(), ConsoleGraphX_Internal::Screen::GetHeight_A(), Vector3());

		_s_backupCamera.insert(backupCam);

	}

	void CameraSystem::Update(float deltaTime)
	{}

	void CameraSystem::_AddCameraToActive(Camera* cam)
	{
		CGX_VERIFY(cam);

		_s_activeCameras.insert(cam);
	}

	void CameraSystem::_RemoveCameraFromActiveSet(Camera* cam)
	{
		CGX_VERIFY(cam);

		auto it = _s_activeCameras.find(cam);

		if (it != _s_activeCameras.end())
			_s_activeCameras.erase(it);
	}

	void CameraSystem::_DeregisterCameraAttachedToEntity(Entity* entity)
	{
		CGX_VERIFY(entity);

		if (Camera* cam = entity->GetComponent<Camera>())
			CameraSystem::DeregisterCamera(cam);
	}

	void CameraSystem::RegisterCamera(Camera* cam)
	{
		CGX_VERIFY(cam);

		_s_cameras.insert(cam);
	}

	void CameraSystem::DeregisterCamera(Camera* cam)
	{
		CGX_VERIFY(cam);

		auto itCam = _s_cameras.find(cam);

		if (itCam != _s_cameras.end())
			_s_cameras.erase(itCam);

		_RemoveCameraFromActiveSet(cam);
	}

	void CameraSystem::SetCameraState(Camera* cam, bool state)
	{
		CGX_VERIFY(cam);

		auto itCam = _s_cameras.find(cam);

		if (itCam == _s_cameras.end())
		{
			ConsoleGraphX_Internal::Debugger::S_LogMessage("When using set camera state, the camera must be registered already");
			return;
		}

		if (state)
			_AddCameraToActive(cam);
		else
			_RemoveCameraFromActiveSet(cam);
	}

	const std::unordered_set<Camera*> CameraSystem::GetActiveCameras()
	{
		if (_s_activeCameras.size() == 0)
		{
			ConsoleGraphX_Internal::Debugger::S_LogMessage("[CAMERA SYSTEM], using backup camera!");
			return _s_backupCamera;
		}

		return _s_activeCameras;
	}
};