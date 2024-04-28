#pragma once
#include "script.h"
#include "entity.h"

using namespace ConsoleGraphX;

struct TestScript : public Script
{
public:
	TestScript();

	void Awake(Entity* owner) override;
	void Update(Entity* owner) override;
};