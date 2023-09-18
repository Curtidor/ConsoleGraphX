#pragma once
#include <vector>
#include "sprite.h"
#include "system.h"
#include "entity.h"


class SpriteSystem : public System
{
private:
	static std::vector<Entity*> _s_entities_w_sprites;

public:
	void Initalize() const override;
	void Update() const override;

	static void RegisterEntitySprite(Entity* entity);

	static void DeregisterEntitySprite(Entity* entity);

	static const std::vector<Entity*>& GetEntitySprites(bool layer_sort = false);
};
