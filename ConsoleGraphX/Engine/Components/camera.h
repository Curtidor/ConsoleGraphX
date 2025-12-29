#pragma once
#include "Engine\Components\component.h"
#include "Engine\Math\vector3.h"
#include "Engine\Math\vector2i.h"

namespace ConsoleGraphX
{
	struct Camera : public ConsoleGraphX_Internal::Component
	{
	private:
		int32_t _m_width;
		int32_t _m_height;

		Vector2i _m_viewPort;
		Vector3 _m_position;

	public:
		Camera();
		Camera(int32_t width, int32_t hegith, const Vector3& position);

		void Clone(Camera* camera) const;

		void SetPosition(const Vector3& position);
		void SetCameraSize(int32_t width, int32_t height);

		const Vector2i& GetViewPort() const;
		const Vector3& GetPosition() const;
	};
};
