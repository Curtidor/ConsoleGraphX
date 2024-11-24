#pragma once
#include <array>
#include <unordered_map>
#include <string>
#include <optional>
#include "color.h"

namespace ConsoleGraphX
{
    class Palette
    {
    private:
        std::array<Color_CGX, 16> _m_colors;
        // unsigned shorts are used for indexes as CHAR_INFO attribue use a unsinged short, this allows direct conversion
        std::unordered_map<std::string, unsigned short> _m_nameToIndex;

    public:
        Palette();

        bool AddColor(const std::string& name, const Color_CGX& color, int index);

        unsigned short GetColorIndex(const std::string& name) const;
        std::optional<Color_CGX> GetColor(unsigned short index) const;
        std::optional<Color_CGX> GetColor(const std::string& name) const;
        std::array<Color_CGX, 16>& GetColors();

        static Palette& DefaultPalette();

    public:
        static inline Palette* m_activePalette = &DefaultPalette();
    };
}
