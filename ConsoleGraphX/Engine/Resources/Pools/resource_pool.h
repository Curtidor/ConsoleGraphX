#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <span>
#include <algorithm>
#include <type_traits>
#include <stdexcept>
#include "Engine\Resources\Pools\base_resource_pool.h"
#include "Engine\Graphics\texture.h"

namespace ConsoleGraphX_Internal
{
    template <typename T>
    class ResourcePool : public BaseResourcePool
    {
    private:
        std::unordered_map<ResourceIndex, ResourceIndex> _m_handleToPoolIndex;
        std::vector<ResourceIndex> _m_cachedActiveIndices;
        std::unordered_set<ResourceIndex> _m_openPoolIndexes;
        std::vector<T> _m_pool;
        size_t _m_maxFreeIndexThreshold;
        bool _m_dirtyCache = false;

        ResourceIndex _GetCompressedIndex(ResourceIndex index)
        {
            auto it = _m_handleToPoolIndex.find(index);
            if (it == _m_handleToPoolIndex.end())
            {
                throw std::runtime_error("bad index");
            }
            return it->second;
        }

        void _RebuildActiveIndexCache()
        {
            _m_cachedActiveIndices.clear();
            _m_cachedActiveIndices.reserve(_m_handleToPoolIndex.size());

            for (const auto& [_, index] : _m_handleToPoolIndex)
            {
                _m_cachedActiveIndices.push_back(index);
            }

            _m_dirtyCache = false;
        }

    protected:
        [[nodiscard]] ResourceIndex _GetOpenPoolIndex()
        {
            _m_dirtyCache = true;

            if (_m_openPoolIndexes.size() >= _m_maxFreeIndexThreshold)
            {
                //Compress(); // if needed
            }

            ResourceIndex index;
            if (!_m_openPoolIndexes.empty())
            {
                auto it = _m_openPoolIndexes.begin();
                index = *it;
                _m_openPoolIndexes.erase(it);
            }
            else
            {
                index = _m_pool.size();
            }

            _m_handleToPoolIndex.insert({ index, index });
            return index;
        }

        void _InsertIntoPool(ResourceIndex index, T&& resource)
        {
            if (index == _m_pool.size())
            {
                _m_pool.push_back(std::move(resource));
            }
            else
            {
                _m_pool[index] = std::move(resource);
            }
        }

    public:
        ResourcePool(size_t maxFreeIndexThreshold = 500)
            : _m_maxFreeIndexThreshold(maxFreeIndexThreshold)
        {}

        ~ResourcePool()
        {
            if constexpr (std::is_pointer_v<T>)
            {
                for (T& resource : _m_pool)
                {
                    delete resource;
                }
            }

            _m_pool.clear();
            _m_openPoolIndexes.clear(); // should now be safe
            _m_handleToPoolIndex.clear();
            _m_cachedActiveIndices.clear();
        }

        template <typename... Args>
        ResourceIndex CreateResource(Args&&... args)
        {
            using NonPointerT = typename std::remove_pointer<T>::type;
            static_assert(std::is_constructible_v<NonPointerT, Args...>, "Resource type is not constructible with the provided arguments!");

            ResourceIndex index = _GetOpenPoolIndex();

            if constexpr (std::is_pointer_v<T>)
            {
                _InsertIntoPool(index, new NonPointerT(std::forward<Args>(args)...));
            }
            else
            {
                if (index == _m_pool.size())
                {
                    _m_pool.emplace_back(std::forward<Args>(args)...);
                }
                else
                {
                    _m_pool[index] = T(std::forward<Args>(args)...);
                }
            }

            return index;
        }

        [[nodiscard]] ResourceIndex CloneResource(ResourceIndex index) override
        {
            if constexpr (std::is_same_v<T, Texture>)
            {
                return index;
            }
            else
            {
                ResourceIndex compIndex = CreateResource();

                typename std::remove_pointer<T>::type* originalComponent = GetResourceFromPool(index);
                typename std::remove_pointer<T>::type* clonedComponent = GetResourceFromPool(compIndex);

                if (originalComponent && clonedComponent)
                {
                    originalComponent->Clone(clonedComponent);
                }

                return compIndex;
            }
        }

        // returns the index of the resource in the pool
        [[nodiscard]] ResourceIndex PlaceResourceInPool(T resource)
        {
            ResourceIndex index = _GetOpenPoolIndex();
            _InsertIntoPool(index, std::move(resource));
            return index;
        }

        std::vector<T>* GetPoolItems()
        {
            return &_m_pool;
        }

        std::span<const ResourceIndex> GetActiveIndexSpan()
        {
            if (_m_dirtyCache)
            {
                _RebuildActiveIndexCache();
            }

            return std::span<const ResourceIndex>(_m_cachedActiveIndices);
        }

        typename std::remove_pointer<T>::type* GetResourceFromPool(ResourceIndex index)
        {
            ResourceIndex cmpIndex = _GetCompressedIndex(index);

            if constexpr (std::is_pointer_v<T>)
            {
                return _m_pool[cmpIndex];
            }
            else
            {
                return &_m_pool[cmpIndex];
            }
        }

        void RemoveResourceFromPool(ResourceIndex index) override
        {
            ResourceIndex cmpIndex = _GetCompressedIndex(index);

            // defensive bounds check
            if (cmpIndex >= _m_pool.size())
            {
                std::cerr << "[ResourcePool] Invalid cmpIndex during remove: " << cmpIndex << " (pool size: " << _m_pool.size() << ")\n";
                __debugbreak();
            }

            T& resource = _m_pool[cmpIndex];

            if constexpr (std::is_pointer_v<T>)
            {
                delete resource;
                resource = nullptr;
            }
            else
            {
                resource.~T();
                resource = T();
            }

            if (!_m_openPoolIndexes.insert(cmpIndex).second)
            {
                std::cerr << "[ResourcePool] Tried to re-insert already freed index: " << cmpIndex << "\n";
                __debugbreak();
            }

            _m_handleToPoolIndex.erase(index); //  restored
            _m_dirtyCache = true;
        }

        void Compress()
        {
            if (_m_openPoolIndexes.empty())
                return;

            std::vector<ResourceIndex> openIndices(_m_openPoolIndexes.begin(), _m_openPoolIndexes.end());
            std::sort(openIndices.begin(), openIndices.end());

            size_t nextAvailableIndex = openIndices.front();
            size_t openIndexPos = 0;

            for (size_t i = nextAvailableIndex; i < _m_pool.size(); ++i)
            {
                if (openIndexPos < openIndices.size() && i == openIndices[openIndexPos])
                {
                    ++openIndexPos;
                }
                else
                {
                    if (nextAvailableIndex != i)
                    {
                        _m_pool[nextAvailableIndex] = std::move(_m_pool[i]);

                        for (auto& entry : _m_handleToPoolIndex)
                        {
                            if (entry.second == i)
                            {
                                entry.second = nextAvailableIndex;
                                break;
                            }
                        }
                    }
                    ++nextAvailableIndex;
                }
            }

            _m_pool.erase(_m_pool.begin() + nextAvailableIndex, _m_pool.end());
            _m_openPoolIndexes.clear();
            _m_dirtyCache = true;
        }
    };
}
