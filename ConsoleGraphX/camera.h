#pragma once
#include "component.h"
#include "vector3.h"

namespace ConsoleGraphX
{
	struct Camera : public ConsoleGraphX_Interal::Component
	{
	private:
		int _m_width;
		int _m_height;

		Vector3 _m_position;

	public:
		Camera();
		Camera(int width, int hegith, const Vector3& position);

		int GetWidth();
		int GetHeight();
		const Vector3 GetPosition();
		void SetPosition(const Vector3& position);
		void SetCameraSize(int width, int height);

		ConsoleGraphX_Interal::Component* Clone() const override;
		int GetID() const override;

	};
};
