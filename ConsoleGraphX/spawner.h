#include "component.h"
#include "entity.h"
#include "vector2.h"
#include "random_numbers.h"
#include "sprite.h"
#include "script.h"

struct Spawner : Component
{
private:
	// spawner is not responsible for memory
	Entity* _m_prefab;
	bool _m_isEnabled;

public:
	Vector3 minSpread;
	Vector3 maxSpread;

	Spawner(Entity* entity);

	void SetEnableState(bool state);
	void SetPrefab(Entity* entiy);
	void Spawn(bool enableScript = false);

	int GetID() const override;
};