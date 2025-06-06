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

        _m_nameToIndex[name] = static_cast<uint16_t>(index);
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

    int16_t Palette::GetColorIndex(const std::string& name) const
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
            palette.AddColor("Color 0", Color_CGX(109, 194, 202), 0);  // #6dc2ca
            palette.AddColor("Color 1", Color_CGX(117, 113, 97), 1);   // #757161
            palette.AddColor("Color 2", Color_CGX(109, 170, 44), 2);   // #6daa2c
            palette.AddColor("Color 3", Color_CGX(89, 125, 206), 3);   // #597dce
            palette.AddColor("Color 4", Color_CGX(133, 149, 161), 4);  // #8595a1
            palette.AddColor("Color 5", Color_CGX(218, 212, 94), 5);   // #dad45e
            palette.AddColor("Color 6", Color_CGX(52, 101, 36), 6);    // #346524
            palette.AddColor("Color 7", Color_CGX(208, 70, 72), 7);    // #d04648
            palette.AddColor("Color 8", Color_CGX(48, 52, 109), 8);    // #30346d
            palette.AddColor("Color 9", Color_CGX(133, 76, 48), 9);    // #854c30
            palette.AddColor("Color 10", Color_CGX(20, 12, 28), 10);   // #140c1c
            palette.AddColor("Color 11", Color_CGX(78, 74, 78), 11);   // #4e4a4e
            palette.AddColor("Color 12", Color_CGX(222, 238, 214), 12); // #deeed6
            palette.AddColor("Color 13", Color_CGX(210, 125, 44), 13); // #d27d2c
            palette.AddColor("Color 14", Color_CGX(68, 36, 52), 14);   // #442434
            palette.AddColor("Color 15", Color_CGX(210, 170, 153), 15); // #d2aa99

            init = true;
        }


        return palette;
    }




}
