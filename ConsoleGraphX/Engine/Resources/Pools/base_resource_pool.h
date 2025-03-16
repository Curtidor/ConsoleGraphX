#pragma once
#include <queue>

namespace ConsoleGraphX_Internal
{
    using ResourceIndex = size_t;

    /**
     * @brief Abstract base class for component pools.
     *
     * This class provides a common non templated interface for managing component pools.
     */
    class BaseResourcePool
    {
    protected:
        std::queue<ResourceIndex> _m_openPoolIndexes;


    public:
        virtual ~BaseResourcePool() = default;
       
        virtual void RemoveResourceFromPool(ResourceIndex index) = 0;
        
        [[nodiscard]] virtual ResourceIndex CloneResource(ResourceIndex index) = 0;
    };
};