#include "CGXPCH.h"
#include "base_resource_pool.h"
#include "resource_id.h"
#include "resource_manager.h"
#include "script.h"
#include "meta_utils.h"

namespace ConsoleGraphX_Internal
{
    BaseResourcePool& ResourceManager::GetResourcePoolFromId(ResourceID id)
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

    void ResourceManager::DestroyEntityResources(const std::unordered_map<ResourceID, ResourceIndex>& componentIdToIndexMap)
    {
        for (const auto& pair : componentIdToIndexMap)
        {
            // if component is a Script or a user-defined component (custom script)
            ResourceID compID = IsScriptFromID(pair.first) ? GenResourceID::Get<ConsoleGraphX::Script>() : pair.first;
            
            //_m_resourcePools[compID]->RemoveResourceFromPool(pair.second);
        }
    }
};
