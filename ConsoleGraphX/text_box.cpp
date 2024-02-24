#include "text_box.h"

TextBox::TextBox(int width, int height) : GUIElement(width, height), m_wrap_text(true)
{}
TextBox::TextBox(int width, int height, int color) : GUIElement(width, height, color), m_wrap_text(true)
{}
TextBox::TextBox(int width, int height, CHAR_INFO* pixels) : GUIElement(width, height, pixels), m_wrap_text(true)
{}

void TextBox::SetText(const std::string& text)
{

}