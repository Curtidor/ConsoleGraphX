#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <cassert>
#include "base_resource_pool.h"
#include "resource_id.h"
#include "resourcec_manager.h"
#include "script.h"

namespace ConsoleGraphX_Internal
{
    ResourceManager::ResourceManager()
    {
        _BuildResourcePoolArray();
    }

    void ResourceManager::_BuildResourcePoolArray()
    {
        using TupleT = BuiltInResoruceTypes::type;
        _LoopOverTuple<TupleT>(std::make_index_sequence<std::tuple_size_v<TupleT>>{});
    }

    void ResourceManager::Initialize()
    {
        assert(!_s_instance);

        _s_instance = new ResourceManager();
    }

    ResourceManager& ResourceManager::Instance()
    {
        assert(_s_instance);

        return *_s_instance;
    }

    void ResourceManager::ShutDown()
    {
        for (auto* pool : _s_resourcePools)
        {
            delete pool;
        }

        delete _s_instance;
        _s_instance = nullptr;
    }

    BaseResourcePool* ResourceManager::GetResourcePoolFromId(ResourceID id)
    {
        #ifdef _DEBUG
        if (id < 0 || id >= BuiltInResoruceTypes::count)
            throw std::runtime_error("Invalid Index!");
        #endif

        return _s_resourcePools[id];
    }

    void ResourceManager::DestroyEntityResources(const std::unordered_map<ResourceID, ResourceIndex>& componentIdToIndexMap)
    {
        for (const auto& pair : componentIdToIndexMap)
        {
            // if component is a Script or a user-defined component (custom script)
            ResourceID compID = IsScriptFromID(pair.first) ? GenResourceID::Get<ConsoleGraphX::Script>() : pair.first;
            
            _s_resourcePools[compID]->RemoveResourceFromPool(pair.second);
        }
    }
};
