#include "snow.h"
#include "debugger.h"
#include "entity.h"
#include "random_numbers.h"
#include "script.h"
#include "transform.h"
#include "vector3.h"

using namespace ConsoleGraphX;

Snow::Snow() : Script(), _m_transform(nullptr)
{}

Snow::Snow(Entity* owner): Script(owner), _m_transform(nullptr)
{}

Snow::Snow(const Snow& other, Entity* owner) : Script(other, owner), _m_transform(nullptr)
{}

void Snow::Clone(Script*& script)
{
	script = new Snow(*this, const_cast<Entity*>(script->GetOwner()));
}

void Snow::Clone(Script*& script, Entity* owner)
{
	script = new Snow(*this, owner);
}

void Snow::Awake()
{
	_m_transform = _m_owner->GetComponent<Transform>();
}

void Snow::Update(float deltaTime)
{
	_m_transform->m_position += Vector3(0, 0.25f, 0);
	ConsoleGraphX_Internal::Debugger::S_LogMessage("SNOW");


	if (_m_transform->m_position.y >= 109)
	{
		if (_m_owner->m_id == 0)
		{
			ConsoleGraphX_Internal::Debugger::S_LogMessage("prefab died");
		}

		Vector3 minSpread = Vector3(0, 0, 0);
		Vector3 maxSpread = Vector3(300, 80, 0);

		float x = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.x, maxSpread.x);
		float y = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.y, maxSpread.y);
		float z = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.z, maxSpread.z);

		_m_transform->SetPosition(x,y,z);
	}
}