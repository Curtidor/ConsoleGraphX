#pragma once
#include "sprite.h"
#include "script.h"


struct Snow : public Script
{
private:
	Transform* _m_transform;

public:
	Snow();

	void Awake(Entity* owner) override;
	void Update(Entity* owner) override;
};