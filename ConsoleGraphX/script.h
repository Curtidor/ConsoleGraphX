#pragma once
#include "component.h"
#include "entity.h"

struct Script : public Component
{
protected:
	Entity* m_entity;

public:
	Script(Entity* entity);

	virtual void Awake();
	virtual void Start();
	virtual bool Update();

	int GetID() const override;
};