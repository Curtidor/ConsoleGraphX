#pragma once

#include <fstream>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <string>
#include "Engine/Graphics/texture.h"
#include "Engine/Core/Utils/file_utils.h"
#include "Engine/Resources/Loaders/texture_loader.h"
#include "WinCore.h"
#include "Engine/Math/vector3.h"

namespace ConsoleGraphX_Internal
{
    struct MapInfo
    {
        uint32_t m_chunk_width;
        uint32_t m_chunk_height;
        uint16_t m_version_info;
    };

    struct MapSprite
    {
        ConsoleGraphX::Vector3 m_mapPosition;
        int m_spriteId;
    };

    struct Chunk
    {
        std::unique_ptr<MapSprite[]> m_sprites;
        size_t m_spriteCount;
    };

    void _ReadMapHeader(MapInfo& mInfo, std::ifstream& file);

    std::vector<Chunk> LoadMap(const std::string& filename);
}

// this function is in the global namespace
std::unordered_map<uint32_t, std::string> LoadSpriteRegistry(const std::string& filePath);