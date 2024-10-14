#pragma once
#include "../ConsoleGraphX/script.h"
#include "../ConsoleGraphX/transform.h"

using namespace ConsoleGraphX;

struct Snow : public Script
{
private:
	Transform* _m_transform;

public:
	Snow();
	Snow(Entity* owner);
	Snow(const Snow& other, Entity* owner);

	void Awake() override;
	void Update(float deltaTime) override;

	void Clone(Script*& script) override;
	void Clone(Script*& script, Entity* owner) override;
};