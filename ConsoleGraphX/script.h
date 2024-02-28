#pragma once
#include "component.h"
#include "entity.h"
#include "dispatcher.h"

struct Script : public Component
{
protected:
	bool m_is_enabled;

public:
	Script();

	virtual void Awake(Entity* owner);
	virtual void Start(Entity* owner);
	virtual void Update(Entity* owner);

	bool IsEnabled();

	int GetID() const override;
};