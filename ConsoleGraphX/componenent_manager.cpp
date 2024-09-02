#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <cassert>
#include "base_component_pool.h"
#include "component_id.h"
#include "component_manager.h"
#include "script.h"

namespace ConsoleGraphX_Internal
{
    ComponentManager::ComponentManager()
    {
        _BuildComponentPoolArray();
    }

    void ComponentManager::_BuildComponentPoolArray()
    {
        using TupleT = BuiltInComponentTypes::type;
        _LoopOverTuple<TupleT>(std::make_index_sequence<std::tuple_size_v<TupleT>>{});
    }

    void ComponentManager::Initialize()
    {
        assert(!_s_Instance);

        _s_Instance = new ComponentManager();
    }

    ComponentManager& ComponentManager::Instance()
    {
        assert(_s_Instance);

        return *_s_Instance;
    }

    void ComponentManager::ShutDown()
    {
        for (auto* pool : _s_componentPools)
        {
            delete pool;
        }

        delete _s_Instance;
        _s_Instance = nullptr;
    }

    BaseComponentPool* ComponentManager::GetComponentPoolFromId(ComponentID id)
    {
        #ifdef _DEBUG
        if (id < 0 || id >= BuiltInComponentTypes::count)
            throw std::runtime_error("Invalid Index!");
        #endif

        return _s_componentPools[id];
    }

    void ComponentManager::DestroyEntityComponents(const std::unordered_map<ComponentID, ComponentIndex>& componentIdToIndexMap)
    {
        for (const auto& pair : componentIdToIndexMap)
        {
            // if component is a Script or a user-defined component (custom script)
            ComponentID compID = IsScriptFromID(pair.first) ? GenComponentID::Get<ConsoleGraphX::Script>() : pair.first;
            
            _s_componentPools[compID]->RemoveComponentFromPool(pair.second);
        }
    }
};
