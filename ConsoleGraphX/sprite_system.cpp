#include <set>
#include <vector>
#include <string>
#include "verify_macro.h"
#include "dispatcher.h"
#include "entity.h"
#include "sprite.h"
#include "sprtite_system.h"


namespace ConsoleGraphX_Interal
{
	bool SpriteComparator::operator()(const ConsoleGraphX::Entity* entityAC, const ConsoleGraphX::Entity* entityBC) const {
		ConsoleGraphX::Entity* entityA = const_cast<ConsoleGraphX::Entity*>(entityAC);
		ConsoleGraphX::Entity* entityB = const_cast<ConsoleGraphX::Entity*>(entityBC);

		ConsoleGraphX::Sprite* spriteA = entityA->GetComponent<ConsoleGraphX::Sprite>();
		ConsoleGraphX::Sprite* spriteB = entityB->GetComponent<ConsoleGraphX::Sprite>();
		
		CGX_VERIFY(spriteA);
		CGX_VERIFY(spriteB);

		if (spriteA->m_layer != spriteB->m_layer) {
			return spriteA->m_layer < spriteB->m_layer;
		}
		else {
			return entityA->m_id < entityB->m_id;
		}
	}

	std::multiset<ConsoleGraphX::Entity*, SpriteComparator> SpriteSystem::_s_entitySprites;

	void SpriteSystem::Initialize()
	{
		std::string objectName = typeid(ConsoleGraphX::Sprite).name();

		Dispatcher<ConsoleGraphX::Entity*>::RegisterListener("AddComponent" + objectName, SpriteSystem::RegisterEntitySprite);
		Dispatcher<ConsoleGraphX::Entity*>::RegisterListener("RemoveComponent" + objectName, SpriteSystem::DeregisterEntitySprite);
	}

	void SpriteSystem::Update(float delta_time) 
	{

	}


	void SpriteSystem::RegisterEntitySprite(ConsoleGraphX::Entity* entity)
	{
		_s_entitySprites.insert(entity);
	}

	void SpriteSystem::DeregisterEntitySprite(ConsoleGraphX::Entity* entity)
	{
		// TEMP
		// when using .find on _s_entitySprite it sometimes returns "end" even though the entity is present
		// until we fix the SpriteComparator we will use this code for removing entities from the multiset
		auto equalsEntity = [entity](ConsoleGraphX::Entity* entry) {
			return entry == entity;
			};

		auto it = std::find_if(_s_entitySprites.begin(), _s_entitySprites.end(), equalsEntity);

		if (it != _s_entitySprites.end()) {
			_s_entitySprites.erase(it);
		}
	}

	std::vector<ConsoleGraphX::Entity*> SpriteSystem::GetEntitySprites()
	{
		std::vector<ConsoleGraphX::Entity*> entitiesVector;
		entitiesVector.reserve(_s_entitySprites.size());

		entitiesVector.insert(entitiesVector.end(), _s_entitySprites.begin(), _s_entitySprites.end());

		return entitiesVector;
	}
};


