#include "gui_element.h"

class TextBox: public GUIElement
{
private:
	std::string _m_text;

public:
	bool m_wrap_text;

	TextBox(int width, int height);
	TextBox(int width, int height, int color);
	TextBox(int width, int height, CHAR_INFO* pixels);

	void SetText(const std::string& text);
};