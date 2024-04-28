#include "snow.h"
#include "component.h"
#include "debugger.h"
#include "entity.h"
#include "random_numbers.h"
#include "script.h"
#include "transform.h"
#include "vector3.h"

using namespace ConsoleGraphX;


Snow::Snow(): Script(), _m_transform(nullptr)
{}

void Snow::Awake(Entity* owner)
{
	this->_m_transform = (Transform*)owner->GetComponent<Transform>();
}

void Snow::Update(Entity* owner)
{

	this->_m_transform->m_position += Vector3(0, 0.05, 0);


	if (this->_m_transform->m_position.y >= 94)
	{
		if (owner->m_id == 0)
		{
			ConsoleGraphX_Interal::Debugger::S_LogMessage("prefab died");
		}

		Vector3 minSpread = Vector3(0, 0, 0);
		Vector3 maxSpread = Vector3(270, 80, 0);

		float x = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.x, maxSpread.x);
		float y = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.y, maxSpread.y);
		float z = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.z, maxSpread.z);

		_m_transform->SetPosition(x,y,z);
	}

}

ConsoleGraphX_Interal::Component* Snow::Clone() const
{
	Snow* clone = new Snow();

	return clone;
}



