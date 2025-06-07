#include "map_loader.h"
#include <sstream>
#include <iostream>

namespace ConsoleGraphX_Internal
{
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
                int32_t x = 0, y = 0, z = 0, spriteId = 0;

                mapFile.read(reinterpret_cast<char*>(&x), sizeof(x));
                mapFile.read(reinterpret_cast<char*>(&y), sizeof(y));
                mapFile.read(reinterpret_cast<char*>(&spriteId), sizeof(spriteId));

                chunkSprites[i] = {
                    ConsoleGraphX::Vector3{
                        static_cast<float>(x),
                        static_cast<float>(y),
                        static_cast<float>(z)
                    },
                    spriteId
                };
            }

            uint64_t terminator = 0;
            mapFile.read(reinterpret_cast<char*>(&terminator), sizeof(terminator));

            chunks.push_back({ std::move(chunkSprites), spriteCount });
        }

        mapFile.close();
        return chunks;
    }
}

std::unordered_map<uint32_t, std::string> LoadSpriteRegistry(const std::string& filePath)
{
    std::unordered_map<uint32_t, std::string> spriteMap;
    std::ifstream file(filePath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open sprite registry: " << filePath << "\n";
        return spriteMap;
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string keyStr, value;
        if (std::getline(ss, keyStr, '=') && std::getline(ss, value)) {
            uint32_t id = static_cast<uint32_t>(std::stoul(keyStr));
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            spriteMap[id] = value;
        }
    }

    return spriteMap;
}
