#pragma once
#include "script.h"


struct TestScript : public Script
{
public:
	TestScript();

	void Awake(Entity* owner) override;
	void Update(Entity* owner) override;
};