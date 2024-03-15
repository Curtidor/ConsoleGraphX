#pragma once
#include "component.h"
#include "vector3.h"

struct Camera : public Component
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

	Component* Clone() const override;
	int GetID() const override;

};