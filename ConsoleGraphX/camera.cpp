#include "camera.h"
#include "component.h"
#include "vector3.h"

namespace ConsoleGraphX
{
    Camera::Camera() : _m_width(0), _m_height(0), _m_position(Vector3()) {

    }

    Camera::Camera(int width, int height, const Vector3& position)
        : _m_width(width), _m_height(height), _m_position(position)
    {}

    const Vector3 Camera::GetPosition() {
        return _m_position;
    }

    void Camera::SetPosition(const Vector3& position)
    {
        _m_position.x = position.x;
        _m_position.y = position.y;
        _m_position.z = position.z;
    }

    int Camera::GetWidth() {
        return _m_width;
    }

    int Camera::GetHeight() {
        return _m_height;
    }

    void Camera::SetCameraSize(int width, int height) {
        _m_width = width;
        _m_height = height;
    }

    ConsoleGraphX_Internal::Component* Camera::Clone() const {
        return new Camera(*this);
    }

    int Camera::GetID() const {
        return ComponentID::camera;
    }

};

