#pragma once

class GameObject
{
protected:
	virtual bool Awake() { return false; }
	virtual bool Start() { return false; }
	virtual bool Update() { return false; }
};