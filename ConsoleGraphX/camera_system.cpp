#include <unordered_set>
#include <string>
#include <stdexcept>
#include "camera_system.h"
#include "camera.h"
#include "debugger.h"
#include "dispatcher.h"
#include "entity.h"
#include "screen.h"
#include "vector3.h"

namespace ConsoleGraphX
{
	std::unordered_set<Camera*> CameraSystem::_s_backupCamera;
	std::unordered_set<Camera*> CameraSystem::_s_cameras;
	std::unordered_set<Camera*> CameraSystem::_s_activeCameras;

	void CameraSystem::Initialize() const
	{
		std::string objectName = typeid(Camera).name();

		ConsoleGraphX_Interal::Dispatcher<Entity*>::RegisterListener("RemoveComponent" + objectName, _CameraRemovedHandler);
		Camera* cam = new Camera(ConsoleGraphX_Interal::Screen::GetWidth_A(), ConsoleGraphX_Interal::Screen::GetHeight_A(), Vector3());

		_s_backupCamera.insert(cam);

	}

	void CameraSystem::Update(float deltaTime) const
	{}

	void CameraSystem::_AddCameraToActive(Camera* cam)
	{
		if (cam == nullptr)
			throw std::runtime_error("bad camera pointer, camera system");

		_s_activeCameras.insert(cam);
	}

	void CameraSystem::_RemoveCameraFromActive(Camera* cam)
	{
		if (cam == nullptr)
			throw std::runtime_error("bad camera pointer, camera system");

		auto it = _s_activeCameras.find(cam);

		if (it != _s_activeCameras.end())
			_s_activeCameras.erase(it);
	}

	void CameraSystem::_CameraRemovedHandler(Entity* entity)
	{
		if (entity == nullptr)
			throw std::runtime_error("null entity, cam system");

		Camera* cam = entity->GetComponent<Camera>();
		if (cam == nullptr)
			return;

		CameraSystem::DeregisterCamera(cam);
	}

	void CameraSystem::RegisterCamera(Camera* cam)
	{
		if (cam == nullptr)
			throw std::runtime_error("bad camera pointer, camera system");


		_s_cameras.insert(cam);
	}

	void CameraSystem::DeregisterCamera(Camera* cam)
	{
		if (cam == nullptr)
			throw std::runtime_error("bad camera pointer, camera system");

		auto itCam = _s_cameras.find(cam);

		if (itCam != _s_cameras.end())
			_s_cameras.erase(itCam);

		_RemoveCameraFromActive(cam);
	}

	void CameraSystem::SetCameraState(Camera* cam, bool state)
	{
		if (cam == nullptr)
			throw std::runtime_error("bad camera pointer, camera system");

		auto itCam = _s_cameras.find(cam);

		if (itCam == _s_cameras.end())
		{
			ConsoleGraphX_Interal::Debugger::S_LogMessage("When using set camera state, the camera must be registered already");
			return;
		}

		if (state)
			_AddCameraToActive(cam);
		else
			_RemoveCameraFromActive(cam);
	}

	const std::unordered_set<Camera*> CameraSystem::GetActiveCameras()
	{
		if (_s_activeCameras.size() == 0)
		{
			ConsoleGraphX_Interal::Debugger::S_LogMessage("[CAMERA SYSTEM], using backup camera!");
			return _s_backupCamera;
		}

		return _s_activeCameras;
	}
};