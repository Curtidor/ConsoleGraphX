#include "PCH_CGX.h"
#include "Engine\Graphics\color.h"
#include "Engine\Graphics\palette.h"


namespace ConsoleGraphX
{
    Palette::Palette() {}

    bool Palette::AddColor(const std::string& name, const Color_CGX& color, int index)
    {
        if (index < 0 || index > _m_colors.size())
        {
            return false;
        }

        _m_nameToIndex[name] = index;
        _m_colors[index] = color;

        return true;
    }

    std::optional<Color_CGX> Palette::GetColor(const std::string& name) const
    {
        auto it = _m_nameToIndex.find(name);
        if (it != _m_nameToIndex.end())
        {
            return _m_colors[it->second];
        }
        return std::nullopt;
    }

    std::optional<Color_CGX> Palette::GetColor(unsigned short index) const
    {
       if (index > 0 && index < _m_colors.size())
       {
           return _m_colors[index];
       }

       return std::nullopt;
    }

    unsigned short Palette::GetColorIndex(const std::string& name) const
    {
        auto it = _m_nameToIndex.find(name);
        if (it != _m_nameToIndex.end())
        {
            return it->second;
        }
        return -1;
    }

    std::array<Color_CGX, 16>& Palette::GetColors()
    {
        return _m_colors;
    }

    Palette& Palette::DefaultPalette()
    {
        static Palette palette;
        static bool init = false;
        if (!init)
        {
            palette.AddColor("Soft Peach", Color_CGX(242, 192, 162), 0);        // #f2c0a2
            palette.AddColor("Coral Pink", Color_CGX(233, 132, 114), 1);        // #e98472
            palette.AddColor("Bright Red", Color_CGX(216, 35, 35), 2);          // #d82323
            palette.AddColor("Deep Maroon", Color_CGX(152, 24, 60), 3);         // #98183c
            palette.AddColor("Bright Green", Color_CGX(31, 203, 35), 4);        // #1fcb23
            palette.AddColor("Forest Green", Color_CGX(18, 109, 48), 5);        // #126d30
            palette.AddColor("Cyan", Color_CGX(38, 221, 221), 6);               // #26dddd
            palette.AddColor("Deep Blue", Color_CGX(24, 103, 160), 7);          // #1867a0
            palette.AddColor("Chestnut Brown", Color_CGX(147, 66, 38), 8);      // #934226
            palette.AddColor("Dark Brown", Color_CGX(108, 37, 30), 9);          // #6c251e
            palette.AddColor("Golden Yellow", Color_CGX(247, 226, 108), 10);    // #f7e26c
            palette.AddColor("Mustard Yellow", Color_CGX(237, 179, 41), 11);    // #edb329
            palette.AddColor("Burnt Orange", Color_CGX(231, 109, 20), 12);      // #e76d14
            palette.AddColor("Soft White", Color_CGX(242, 242, 249), 13);       // #f2f2f9
            palette.AddColor("Lavender Blue", Color_CGX(106, 95, 160), 14);     // #6a5fa0
            palette.AddColor("Deep Black", Color_CGX(22, 20, 35), 15);          // #161423

            init = true;
        }

        return palette;
    }




}
