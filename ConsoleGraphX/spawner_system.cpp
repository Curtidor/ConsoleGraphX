#include "spawner_system.h"

std::unordered_map<Spawner*, Entity*> SpawnerSystem::_s_entitySpawnerPairs;

void SpawnerSystem::Initialize() const
{
	Dispatcher<Entity*>::RegisterListener("AddComponentstruct Spawner", RegisterSpawner);
	Dispatcher<Entity*>::RegisterListener("RemoveComponentstruct Spawner", DeregisterSpawner);
	Dispatcher<Spawner*>::RegisterListener("SpawnedEntity", SpawnEntity);
}

void SpawnerSystem::Update(float delta_time) const {};


void SpawnerSystem::SpawnEntity(Spawner* spawner)
{
	auto it = SpawnerSystem::_s_entitySpawnerPairs.find(spawner);

	if (it == SpawnerSystem::_s_entitySpawnerPairs.end())
	{
		Debugger::S_LogMessage("[SPAWNER SYSTEM] cant find spawner");
		return;
	}

	Entity* prefab = it->second;
	Entity* spawnedEntity = new Entity(prefab->m_name);

	SpawnerSystem::CloneEntity(prefab, spawnedEntity);

	float x = RandomNumberGenerator::GenerateRandomFloatInRange(it->first->minSpread.x, it->first->maxSpread.x);
	float y = RandomNumberGenerator::GenerateRandomFloatInRange(it->first->minSpread.y, it->first->maxSpread.y);
	float z = RandomNumberGenerator::GenerateRandomFloatInRange(it->first->minSpread.z, it->first->maxSpread.z);


	Vector3 prefabPosition = prefab->GetComponent<Transform>()->GetPosition();
	Vector3 spawnPosition = Vector3(x, y, z) + prefabPosition;
	spawnedEntity->GetComponent<Transform>()->SetPosition(spawnPosition);

	Dispatcher<Entity*>::Notify("EntityCreation", spawnedEntity);
}

void SpawnerSystem::CloneEntity(Entity* prefab, Entity* spawnedEntity)
{
	for (std::pair<std::type_index, Component*> componentPair : prefab->GetComponents())
	{
		// spawner's cant spawn spawner's and the entity class is responsible for adding transform so there's no need to clone a new one
		if (componentPair.second->GetID() == ComponentID::spawner || componentPair.second->GetID() == ComponentID::transform)
			continue;

		Component* clonedComponent = componentPair.second->Clone();
		std::string componentName = componentPair.second->ComponentName();

		spawnedEntity->AddComponentClone(clonedComponent, componentName, componentPair.first);
	}
}

void SpawnerSystem::RegisterSpawner(Entity* prefab)
{
	if (prefab == nullptr)
	{
		Debugger::S_LogMessage("[SPAWNER SYSTEM] null entity");
		return;
	}

	if (Spawner* spawner = (Spawner*)prefab->GetComponent<Spawner>())
	{
		SpawnerSystem::_s_entitySpawnerPairs.insert(std::make_pair(spawner, prefab));
	}
}

void SpawnerSystem::DeregisterSpawner(Entity* entity)
{

}

