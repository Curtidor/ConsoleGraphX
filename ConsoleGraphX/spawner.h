#pragma once
#include "component.h"
#include "entity.h"

struct Spawner : Component
{
private:
	// spawner is not responsible for memory
	bool _m_isEnabled;

public:
	Vector3 minSpread;
	Vector3 maxSpread;

	Spawner();

	void SetEnableState(bool state);
	void Spawn();

	Component* Clone() const override;
	int GetID() const override;

};