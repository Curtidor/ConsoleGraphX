#include "CGXPCH.h"
#include "color.h"
#include "palette.h"


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
            palette.AddColor("Deep Plum", Color_CGX(20, 12, 28), 0);           // (20, 12, 28)
            palette.AddColor("Dark Wine", Color_CGX(68, 36, 52), 1);           // (68, 36, 52)
            palette.AddColor("Olive Beige", Color_CGX(117, 113, 97), 2);       // (117, 113, 97)
            palette.AddColor("Copper Brown", Color_CGX(133, 76, 48), 3);       // (133, 76, 48)
            palette.AddColor("Muted Gray", Color_CGX(78, 74, 78), 4);          // (78, 74, 78)
            palette.AddColor("Crimson Red", Color_CGX(208, 70, 72), 5);        // (208, 70, 72)
            palette.AddColor("Burnt Orange", Color_CGX(210, 125, 44), 6);      // (210, 125, 44)
            palette.AddColor("Forest Green", Color_CGX(52, 101, 36), 7);       // (52, 101, 36)
            palette.AddColor("Royal Blue", Color_CGX(48, 52, 109), 8);         // (48, 52, 109)
            palette.AddColor("Light Rose", Color_CGX(210, 170, 153), 9);       // (210, 170, 153) // hair
            palette.AddColor("Cool Blue Gray", Color_CGX(133, 149, 161), 10);  // (133, 149, 161)
            palette.AddColor("Pale Mint", Color_CGX(222, 238, 214), 11);       // (222, 238, 214)
            palette.AddColor("Teal Blue", Color_CGX(0, 0, 0), 12);       // (109, 194, 202)
            palette.AddColor("Steel Blue", Color_CGX(89, 125, 206), 13);       // (89, 125, 206)
            palette.AddColor("Golden Yellow", Color_CGX(218, 212, 94), 14);    // (218, 212, 94) //eyes
            palette.AddColor("Lime Green", Color_CGX(109, 170, 44), 15);       // (109, 170, 44)

            init = true;
        }

        return palette;
    }



}
