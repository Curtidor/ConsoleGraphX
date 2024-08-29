#include "gui_element.h"

namespace ConsoleGraphX_GUI
{
	GUIElement::GUIElement(int width, int hegiht, const std::string& name) : Entity(name)
	{
		_m_sprite = (ConsoleGraphX::Sprite*)AddComponent<ConsoleGraphX::Sprite>(width, hegiht, ConsoleGraphX::Color::Magenta);
	}

	GUIElement::GUIElement(int width, int hegiht, int color, const std::string& name) : Entity(name)
	{
		_m_sprite = (ConsoleGraphX::Sprite*)AddComponent<ConsoleGraphX::Sprite>(width, hegiht, color);
	}

	GUIElement::GUIElement(int width, int hegiht, CHAR_INFO* pixels, const std::string& name) : Entity(name)
	{
		_m_sprite = (ConsoleGraphX::Sprite*)AddComponent<ConsoleGraphX::Sprite>(width, hegiht, pixels);

	}
}


