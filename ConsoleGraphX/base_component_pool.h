#pragma once
#include <queue>

namespace ConsoleGraphX_Internal
{
    using ComponentIndex = size_t;

    /**
     * @brief Abstract base class for component pools.
     *
     * This class provides a common non templated interface for managing component pools.
     */
    class BaseComponentPool
    {
    protected:
        std::queue<ComponentIndex> _m_openPoolIndexes;

    public:
        virtual ~BaseComponentPool() = default;
        /**
         * @brief Removes a component at the given index.
         *
         * This method should be implemented by derived classes.
         *
         * @param index The index of the component to remove.
         */
        virtual void RemoveComponentFromPool(ComponentIndex index) = 0;
        
        [[nodiscard]] virtual ComponentIndex CloneComponent(ComponentIndex index) = 0;
    };
};