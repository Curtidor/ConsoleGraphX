#include "gui_element.h"

GUIElement::GUIElement(int width, int hegiht, const std::string& name): Entity(name)
{
	this->_m_sprite = (Sprite*)this->AddComponent<Sprite>(width, hegiht, Color::Magenta);
}

GUIElement::GUIElement(int width, int hegiht, int color, const std::string& name): Entity(name)
{
	this->_m_sprite = (Sprite*)this->AddComponent<Sprite>(width, hegiht, color);
}

GUIElement::GUIElement(int width, int hegiht, CHAR_INFO* pixels, const std::string& name) : Entity(name)
{
	this->_m_sprite = (Sprite*)this->AddComponent<Sprite>(width, hegiht, pixels);

}

