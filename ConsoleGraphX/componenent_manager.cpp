#include <stdexcept>
#include <unordered_map>
#include <utility>
#include "base_component_pool.h"
#include "component_id.h"
#include "component_manager.h"


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

    ComponentManager& ComponentManager::Instance()
    {
        static ComponentManager instance;
        return instance;
    }

    void ComponentManager::FreePools()
    {
        for (auto* pool : _m_componentPools)
        {
            delete pool;
        }
    }

    BaseComponentPool* ComponentManager::GetComponentPoolFromId(ComponentID id)
    {
        #ifdef _DEBUG
        if (id < 0 || id >= BuiltInComponentTypes::count)
            throw std::runtime_error("Invalid Index!");
        #endif

        return _m_componentPools[id];
    }

    void ComponentManager::DestroyEntityComponents(const std::unordered_map<ComponentID, ComponentIndex>& componentIdToIndexMap)
    {
        for (const auto& pair : componentIdToIndexMap)
        {
            // if component is a Script or a user-defined component (custom script)
            ComponentID compID = IsScriptFromID(pair.first) ? GenComponentID::Get<ConsoleGraphX::Script>() : pair.second;
            
            _m_componentPools[compID]->RemoveComponentFromPool(pair.second);
        }
    }
};
