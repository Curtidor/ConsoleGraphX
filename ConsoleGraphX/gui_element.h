#pragma once
#include <vector>
#include "sprite.h"
#include "entity.h"

namespace ConsoleGraphX_GUI
{
	class GUIElement : public ConsoleGraphX::Entity
	{
	private:
		std::vector<GUIElement> _m_children;
		ConsoleGraphX::Sprite* _m_sprite;

	public:
		GUIElement(int width, int hegiht, const std::string& name = "");
		GUIElement(int width, int hegiht, int color, const std::string& name = "");
		GUIElement(int width, int hegiht, CHAR_INFO* pixels, const std::string& name = "");

	};
};
