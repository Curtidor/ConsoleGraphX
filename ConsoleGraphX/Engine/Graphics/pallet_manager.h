#pragma once
#include <unordered_map>
#include <string>
#include "Engine\Graphics\palette.h"


namespace ConsoleGraphX
{
	class Palette_Manager
	{
	private:
		std::unordered_map<std::string, Palette> _m_nameToPaletteMap;


	};
};
