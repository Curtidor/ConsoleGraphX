#include "sprtite_system.h"
#include <set>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "dispatcher.h"
#include "entity.h"
#include "sprite.h"

bool SpriteComparator::operator()(const Entity* entityAC, const Entity* entityBC) const {
	Entity* entityA = const_cast<Entity*>(entityAC);
	Entity* entityB = const_cast<Entity*>(entityBC);
	
	Sprite* spriteA = entityA->GetComponent<Sprite>();
	Sprite* spriteB = entityB->GetComponent<Sprite>();

	if (!spriteA || !spriteB) {
		throw std::runtime_error("Null sprite encountered in SpriteComparator");
	}

	if (spriteA->m_layer != spriteB->m_layer) {
		return spriteA->m_layer < spriteB->m_layer;
	} 
	else {
		return entityA->m_id < entityB->m_id;
	}
}

std::multiset<Entity*, SpriteComparator> SpriteSystem::_s_entitySprites;

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
	_s_entitySprites.insert(entity);
}

void SpriteSystem::DeregisterEntitySprite(Entity* entity)
{
	// TEMP
	// when using .find on _s_entitySprite it sometimes returns "end" even though the entity is present
	// until we fix the SpriteComparator we will use this code for removing entities from the multiset
	auto equalsEntity = [entity](Entity* entry) {
		return entry == entity;
	};

	auto it = std::find_if(_s_entitySprites.begin(), _s_entitySprites.end(), equalsEntity);

	if (it != _s_entitySprites.end()) {
		_s_entitySprites.erase(it);
	}
}

std::vector<Entity*> SpriteSystem::GetEntitySprites()
{
	std::vector<Entity*> entitiesVector;
	entitiesVector.reserve(_s_entitySprites.size()); 

	entitiesVector.insert(entitiesVector.end(), _s_entitySprites.begin(), _s_entitySprites.end());

	return entitiesVector;
}

