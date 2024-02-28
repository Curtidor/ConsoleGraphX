#include "snow.h"

Snow::Snow(): Script(), _m_transform(NULL)
{}

void Snow::Awake(Entity* owner)
{
	this->_m_transform = (Transform*)owner->GetComponent<Transform>();
	this->m_is_enabled = true;
}

void Snow::Update(Entity* owner)
{
	this->_m_transform->m_position += Vector3(0.1, 0, 0);
}



