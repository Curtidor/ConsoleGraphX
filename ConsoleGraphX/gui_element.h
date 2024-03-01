#pragma once
#include <vector>
#include "sprite.h"
#include "entity.h"


class GUIElement : public Entity
{
private:
	std::vector<GUIElement> _m_children;
	Sprite* _m_sprite;

public:
	GUIElement(int width, int hegiht, const std::string& name = "");
	GUIElement(int width, int hegiht, int color, const std::string& name = "");
	GUIElement(int width, int hegiht, CHAR_INFO* pixels, const std::string& name = "");
	
};