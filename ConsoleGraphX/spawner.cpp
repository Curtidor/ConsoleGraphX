#include "spawner.h"

Spawner::Spawner(Entity* prefab) : _m_prefab(prefab), _m_isEnabled(false)
{}

void Spawner::SetEnableState(bool state)
{
	this->_m_isEnabled = state;
}

void Spawner::SetPrefab(Entity* prefab)
{
	this->_m_prefab = prefab;
}

void Spawner::Spawn(bool enableScript)
{
	Entity* spawnedEntity = new Entity(*this->_m_prefab);
	Vector3 positionPrefab = this->_m_prefab->GetComponent<Transform>()->GetPosition();


	float x = RngGenerator::getRandomNumber(this->minSpread.x, this->maxSpread.x) + positionPrefab.x;
	float y = RngGenerator::getRandomNumber(this->minSpread.y, this->maxSpread.y) + positionPrefab.y;
	float z = RngGenerator::getRandomNumber(this->minSpread.z, this->maxSpread.z) + positionPrefab.z;

	std::unordered_map<std::type_index, Component*> prefabComponents = this->_m_prefab->GetComponents();
	std::unordered_map<std::type_index, Component*> otherPrefabComponents = spawnedEntity->GetComponents();

	//Sprite* sprite = (Sprite*)spawnedEntity->AddComponent<Sprite>(10, 10, Color::DarkGray);
	//sprite->m_layer = 2;
	spawnedEntity->GetComponent<Transform>()->SetPosition(x, y, z);


	//for (const std::pair<std::type_index, Component*> comp : this->_m_prefab->GetComponents())
	//{
	//	spawnedEntity->AddComponentCopy(comp.second);
	//}

	//// delete the components that we got from the other entity
	//spawnedEntity->RemoveComponent<Transform>(false);
	//spawnedEntity->RemoveComponent<Sprite>(false);

	//if (Sprite* prefabSprite = (Sprite*)this->_m_prefab->GetComponent<Sprite>())
	//{
	//	spawnedEntity->AddComponent<Sprite>(*prefabSprite);
	//}

	//Script* script = (Script*)spawnedEntity->GetComponentByID(ComponentID::script);
	//if (script != NULL && enableScript)
	//{
	//	script->SetOwner(spawnedEntity);
	//	script->SetStatus(true);
	//}

	//Vector3 postion = Vector3(x, y, z);

	//Transform* spawnEntityTransform = (Transform*)spawnedEntity->AddComponent<Transform>();
	//spawnEntityTransform->SetPosition(Vector3(x, y, z));

	Dispatcher<Entity*>::Notify("SpawnedEntity", spawnedEntity);
}

int Spawner::GetID() const
{
	return ComponentID::spawner;
}