#include "PCH_CGX.h"
#include <fstream>
#include <unordered_map>
#include <vector>
#include "Engine/Resources/Loaders/animation_loader.h"
#include "Engine/Components/sprite_animation.h"
#include "Engine/Resources/Pools/component_texture_pool.h"
#include "Engine/Core/Utils/file_utils.h"

namespace ConsoleGraphX_Internal
{
    struct AnimationInfo
    {
        uint32_t m_totalFrames;
        uint32_t m_totalFiles;
    };

    static void _ReadAnimationHeader(AnimationInfo& animationInfo, std::ifstream& animationFile);
    static ResourceIndex _ReadAnimationAndPlaceInPool(AnimationInfo& animationInfo, std::ifstream& animationFile, ResourceManager& rManager, size_t owningEntity);

    ResourceIndex LoadAnimation(const std::string& path, ResourceManager& resourceManager, size_t owningEntity)
    {
        VerifyFileExtension(path, ".cxaim");

        std::ifstream animationFile = OpenFile(path);

        AnimationInfo animationInfo{};
        _ReadAnimationHeader(animationInfo, animationFile);

        ResourceIndex rIndex = _ReadAnimationAndPlaceInPool(animationInfo, animationFile, resourceManager, owningEntity);

        animationFile.close();
        return rIndex;
    }

    static void _ReadAnimationHeader(AnimationInfo& animationInfo, std::ifstream& animationFile)
    {
        uint32_t version = 0;
        animationFile.read(reinterpret_cast<char*>(&version), sizeof(uint32_t));
        animationFile.read(reinterpret_cast<char*>(&animationInfo.m_totalFrames), sizeof(uint32_t));
        animationFile.read(reinterpret_cast<char*>(&animationInfo.m_totalFiles), sizeof(uint32_t));
    }

    static ResourceIndex _ReadAnimationAndPlaceInPool(AnimationInfo& animationInfo, std::ifstream& animationFile, ResourceManager& rManager, size_t owningEntity)
    {
        std::unordered_map<uint32_t, ResourceIndex> textureIdToPoolIndex{};
        std::vector<Texture>& texturePool = *rManager.GetResourcePool<Texture>().GetPoolItems();

        for (size_t i = 0; i < animationInfo.m_totalFiles; i++)
        {
            uint16_t fileNameLength = 0;
            animationFile.read(reinterpret_cast<char*>(&fileNameLength), sizeof(uint16_t));

            std::string filePath(fileNameLength, '\0');
            animationFile.read(&filePath[0], fileNameLength);

            ResourceIndex rIndex = rManager.CreateTextureResource(filePath).second;
            textureIdToPoolIndex.emplace(texturePool[rIndex].GetId(), rIndex);
        }

        std::vector<ConsoleGraphX::SpriteAnimationFrame> frames;
        for (size_t i = 0; i < animationInfo.m_totalFrames; i++)
        {
            uint32_t textureID = 0;
            uint32_t duration = 0;

            animationFile.read(reinterpret_cast<char*>(&textureID), sizeof(uint32_t));
            animationFile.read(reinterpret_cast<char*>(&duration), sizeof(uint32_t));

            ResourceIndex rIndex = textureIdToPoolIndex[textureID];
            frames.emplace_back(rIndex, duration);
        }

        return rManager.CreateResource<ConsoleGraphX::SpriteAnimation>(std::move(frames), owningEntity).second;
    }
}
