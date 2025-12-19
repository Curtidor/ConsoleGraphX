#include "PCH_CGX.h"
#include <variant>
#include "Engine\Resources\Pools\base_resource_pool.h"
#include "Engine\Resources\resource_id.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Components\script.h"
#include "Engine\Core\Utils\meta_utils.h"
#include "Engine/Resources/Loaders/animation_loader.h"

namespace ConsoleGraphX_Internal
{
    ResourcePoolVariant ResourceManager::GetResourcePoolFromId(ResourceID id)
    {
        return _GetResourcePoolByIndex(_m_resourcePoolsT, id);
    }

    void ResourceManager::SetActiveManager(ResourceManager* manager)
    {
        CGX_VERIFY(manager, "Manager is nullprt");

        _s_activeResourceManager = manager;
    }

    ResourceManager& ResourceManager::GetActiveResourceManager()
    {
        CGX_VERIFY(_s_activeResourceManager, "No active manager set");

        return *_s_activeResourceManager;
    }

    ResourceManager::ResourceManager()
    {
    }

    std::pair<ResourceID, ResourceIndex> ResourceManager::CreateTextureResource(const std::string& filename)
    {
        ComponentTexturePool& tPool = GetResourcePool<Texture>();

        return tPool.LoadTexture(filename);
    }

    std::pair<ResourceID, ResourceIndex> ResourceManager::CreateTextureResource(uint32_t width, uint32_t height, int color)
    {
        Texture* texture = new Texture(width, height, color);

        ComponentTexturePool& tPool = GetResourcePool<Texture>();

        return { GenResourceID::Get<Texture>(), tPool.PlaceResourceInPool(std::move(*texture)) };
    }

    std::pair<ResourceID, ResourceIndex> ResourceManager::CreateAnimationResource(const std::string& path, size_t owningEntity)
    {
        return { GenResourceID::Get<ConsoleGraphX::SpriteAnimation>(), LoadAnimation(path, *this, owningEntity) };
    }


    void ResourceManager::DestroyEntityResources(const std::unordered_map<ResourceID, ResourceIndex>& componentIdToIndexMap)
    {
        for (const auto& pair : componentIdToIndexMap)
        {
            // if component is a Script or a user-defined component (custom script)
            ResourceID compID = IsScriptFromId(pair.first) ? GenResourceID::Get<ConsoleGraphX::Script>() : pair.first;

            auto poolVariant = _GetResourcePoolByIndex(_m_resourcePoolsT, compID);
            std::visit([&pair](auto& poolWrapper) 
                {
                    auto& pool = poolWrapper.get(); // unwrap the reference
                    pool.RemoveResourceFromPool(pair.second);
                }, poolVariant);

        }
    }
};
