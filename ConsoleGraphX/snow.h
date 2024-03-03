#pragma once
#include "script.h"
#include "component.h"
#include "entity.h"
#include "transform.h"


struct Snow : public Script
{
private:
	Transform* _m_transform;

public:
	Snow();

	void Awake(Entity* owner) override;
	void Update(Entity* owner) override;

	Component* Clone() const override;
};