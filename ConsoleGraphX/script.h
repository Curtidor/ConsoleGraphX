#pragma once
#include "component.h"
#include "entity.h"
#include "dispatcher.h"

struct Script : public Component
{
protected:
	Entity* m_entity;
	bool m_is_enabled;

public:
	Script(Entity* entity);

	bool IsEnabled();

	void SetStatus(bool state);

	virtual void Awake();
	virtual void Start();
	virtual void Update();

	int GetID() const override;
};