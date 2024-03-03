#pragma once
#include <set>
#include "system.h"
#include "entity.h"
#include <vector>

struct SpriteComparator {
	bool operator()(const Entity* entityA, const Entity* entityB) const;
};

class SpriteSystem : public System
{
private:
	static std::multiset<Entity*, SpriteComparator> _s_entitySprites;

public:
	void Initialize() const override;
	void Update(float delta_time) const override;

	static void RegisterEntitySprite(Entity* entity);

	static void DeregisterEntitySprite(Entity* entity);

	static std::vector<Entity*> GetEntitySprites();

};
