#pragma once
#include "script.h"
#include "entity.h"
#include "transform.h"

using namespace ConsoleGraphX;

struct Snow : public Script
{
private:
	Transform* _m_transform;

public:
	Snow();

	void Awake(Entity* owner) override;
	void Update(Entity* owner) override;

	void Clone(Snow* snow) const;
};