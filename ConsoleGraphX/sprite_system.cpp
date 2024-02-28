#include "sprtite_system.h"

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
		return entityA->GetId() < entityB->GetId();
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
	auto it = _s_entitySprites.find(entity);

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

