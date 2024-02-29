#pragma once
#include <unordered_map>
#include "system.h"
#include "random_numbers.h"
#include "spawner.h"

class SpawnerSystem : public System
{
private:
	// entity, spawner with entity
	static std::unordered_map<Spawner*, Entity*> _s_entitySpawnerPairs;

	static void SpawnEntity(Spawner* spawner);
	static void CloneEntity(Entity* prefab, Entity* spawnedEntity);

public:
	void Initialize() const override;
	void Update(float delta_time) const override;


	static void RegisterSpawner(Entity* owner);
	static void DeregisterSpawner(Entity* owner);
};