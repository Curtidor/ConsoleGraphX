#include "camera.h"
#include "vector3.h"
#include "vector2.h"

namespace ConsoleGraphX
{
    Camera::Camera() : _m_width(1), _m_height(1), _m_position(Vector3()), _m_viewPort(1, 1)
    {}

    Camera::Camera(int width, int height, const Vector3& position)
        : _m_width(width), _m_height(height), _m_position(position), _m_viewPort(width, height)
    {}

    void Camera::SetPosition(const Vector3& position)
    {
        _m_position.x = position.x;
        _m_position.y = position.y;
        _m_position.z = position.z;
    }

    void Camera::SetCameraSize(int width, int height) 
    {
        _m_width = width;
        _m_height = height;
    }

    const Vector3& Camera::GetPosition() const 
    {
        return _m_position;
    }

    const Vector2& Camera::GetViewPort() const 
    {
        return _m_viewPort;
    }

    void Camera::Clone(Camera* camera) const 
    {
        *camera = *this;
    }
};

