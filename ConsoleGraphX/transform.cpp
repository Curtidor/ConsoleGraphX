#include "transform.h"
#include <iostream>

Transform::Transform()
{
    this->m_position = Vector3();
    this->m_scale = Vector3(1, 1, 1);
    this->m_rotation = 0;
}

Transform::Transform(float x, float y, float scaleX, float scaleY)
{
    this->m_position = Vector3(x, y);
    this->m_scale = Vector3(scaleX, scaleY, 1);
    this->m_rotation = 0;
}

Transform::Transform(float x, float y, float z, float scaleX, float scaleY, float scaleZ)
{
    this->m_position = Vector3(x, y, z);
    this->m_scale = Vector3(scaleX, scaleY, scaleZ);
    this->m_rotation = 0;
}

Transform::~Transform(){}


void Transform::SetPosition(float x, float y)
{
    this->m_position.x = x;
    this->m_position.y = y;
}

void Transform::SetPosition(float x, float y, float z)
{
    this->m_position.x = x;
    this->m_position.y = y;
    this->m_position.z = z;
}

void Transform::SetScale(float x, float y)
{
    this->m_scale.x = x;
    this->m_scale.y = y;
}

void Transform::SetScale(float x, float y, float z)
{
    this->m_scale.x = x;
    this->m_scale.y = y;
    this->m_scale.z = z;
}

const Vector3 Transform::GetPosition() { return this->m_position; }