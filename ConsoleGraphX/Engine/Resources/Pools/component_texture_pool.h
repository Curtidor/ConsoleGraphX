#pragma once
#include "Engine/Graphics/texture.h"
#include "Engine/Resources/Pools/resource_pool.h"
#include "Engine/Resources/texture_loader.h"
#include <Engine/Resources/resource_id.h>

namespace ConsoleGraphX_Internal 
{
    class ComponentTexturePool : public ResourcePool<Texture> 
    {
    public:
        std::pair<ResourceID, ResourceIndex> LoadTexture(const std::string& filename) 
        {
            auto it = textureCache.find(filename);
            if (it != textureCache.end()) 
            {
                return { GenResourceID::Get<Texture>(), it->second };
            }

            std::unique_ptr<Texture> tex(TextureLoader::LoadTexture(filename));
            if (!tex) 
            {
                throw std::runtime_error("Failed to load texture: " + filename);
            }

            ResourceIndex index = PlaceResourceInPool(std::move(*tex));
            textureCache[filename] = index;

            return { GenResourceID::Get<Texture>(), index };
        }

    private:
        std::unordered_map<std::string, ResourceIndex> textureCache;
    };
};
