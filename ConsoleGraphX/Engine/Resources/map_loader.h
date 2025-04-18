#pragma once
#include <iosfwd>
#include <fstream>
#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include "Engine\Graphics\texture.h"
#include "Engine\Core\Utils\file_utils.h"
#include "Engine\Resources\texture_loader.h"
#include "WinCore.h"
#include "Engine\Math\vector3.h"

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

    std::vector<Chunk> LoadMap(const std::string& filename)
    {
        VerifyFileExtension(filename, ".cxmap");

        std::ifstream mapFile = OpenFile(filename);

        MapInfo mapInfo{};
        _ReadMapHeader(mapInfo, mapFile);

        std::vector<Chunk> chunks;

        while (true)
        {
            uint32_t spriteCount = 0;
            mapFile.read(reinterpret_cast<char*>(&spriteCount), sizeof(spriteCount));

            if (mapFile.eof()) break;

            if (spriteCount == 0)
                continue;

            auto chunkSprites = std::make_unique<MapSprite[]>(spriteCount);

            for (uint32_t i = 0; i < spriteCount; ++i)
            {
                int32_t x = 0, y = 0, z = 0;
                int spriteId = 0;

                mapFile.read(reinterpret_cast<char*>(&x), sizeof(x));
                mapFile.read(reinterpret_cast<char*>(&y), sizeof(y));
                mapFile.read(reinterpret_cast<char*>(&spriteId), sizeof(spriteId));

                chunkSprites[i] = { ConsoleGraphX::Vector3{static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)}, spriteId };
            }

            // Read and discard chunk terminator (64-bit)
            uint64_t terminator = 0;
            mapFile.read(reinterpret_cast<char*>(&terminator), sizeof(terminator));

            chunks.push_back({ std::move(chunkSprites), spriteCount });
        }

        mapFile.close();
        return chunks;
    }

    void _ReadMapHeader(MapInfo& mInfo, std::ifstream& file)
    {
        file.read(reinterpret_cast<char*>(&mInfo.m_version_info), sizeof(mInfo.m_version_info));
        file.read(reinterpret_cast<char*>(&mInfo.m_chunk_width), sizeof(mInfo.m_chunk_width));
        file.read(reinterpret_cast<char*>(&mInfo.m_chunk_height), sizeof(mInfo.m_chunk_height));

        if (file.fail())
        {
            throw std::runtime_error("Failed to read map header");
        }
    }

}
