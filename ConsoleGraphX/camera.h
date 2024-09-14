#pragma once
#include "component.h"
#include "vector3.h"
#include "vector2.h"

namespace ConsoleGraphX
{
	struct Camera : public ConsoleGraphX_Internal::Component
	{
	private:
		int _m_width;
		int _m_height;

		Vector2 _m_viewPort;
		Vector3 _m_position;

	public:
		Camera();
		Camera(int width, int hegith, const Vector3& position);

		void Clone(Camera* camera) const;

		void SetPosition(const Vector3& position);
		void SetCameraSize(int width, int height);

		const Vector2& GetViewPort() const;
		const Vector3& GetPosition() const;
	};
};
