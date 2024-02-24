#include "sprtite_system.h"

std::vector<Entity*> SpriteSystem::_s_entities_w_sprites;

void SpriteSystem::Initialize() const
{
	Dispatcher<Entity*>::RegisterListener("AddComponentstruct Sprite", SpriteSystem::RegisterEntitySprite);
	Dispatcher<Entity*>::RegisterListener("RemoveComponentstruct Sprite", SpriteSystem::DeregisterEntitySprite);
}

void SpriteSystem::Update(float delta_time) const
{
	
}


void SpriteSystem::RegisterEntitySprite(Entity* entity)
{
	_s_entities_w_sprites.push_back(entity);
}

void SpriteSystem::DeregisterEntitySprite(Entity* entity)
{
	auto it = std::find(_s_entities_w_sprites.begin(), _s_entities_w_sprites.end(), entity);

	if (it != _s_entities_w_sprites.end()) {
		_s_entities_w_sprites.erase(it);
	}
}

const std::vector<Entity*>& SpriteSystem::GetEntitySprites(bool layer_sort)
{
	if (layer_sort)
	{
		// do layer sort
	}
	return _s_entities_w_sprites;
}

