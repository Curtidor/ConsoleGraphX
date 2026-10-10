#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <span>
#include <type_traits>
#include <stdexcept>
#include <utility>

#include "Engine/Resources/Pools/base_resource_pool.h"
#include "Engine/Graphics/texture.h"

namespace ConsoleGraphX_Internal
{
    template <typename T>
    class ResourcePool : public BaseResourcePool
    {
    private:
        using NonPointerT = typename std::remove_pointer<T>::type;

        // Dense storage: ONLY active resources live here.
        std::vector<T> _m_pool;                       // dense active items
        std::vector<ResourceIndex> _m_denseHandles;   // denseIndex -> handle
        std::unordered_map<ResourceIndex, ResourceIndex> _m_handleToDense; // handle -> denseIndex

        // Reuse handles to avoid ever-growing handle values.
        std::vector<ResourceIndex> _m_freeHandles;
        ResourceIndex _m_nextHandle = 0;

        ResourceIndex _AllocHandle()
        {
            if (!_m_freeHandles.empty())
            {
                ResourceIndex h = _m_freeHandles.back();
                _m_freeHandles.pop_back();
                return h;
            }
            return _m_nextHandle++;
        }

        ResourceIndex _GetDenseIndex(ResourceIndex handle) const
        {
            auto it = _m_handleToDense.find(handle);
            if (it == _m_handleToDense.end())
                throw std::runtime_error("bad index (handle not active)");
            return it->second;
        }

        void _EraseDenseAt(ResourceIndex denseIndex)
        {
            const ResourceIndex last = static_cast<ResourceIndex>(_m_pool.size() - 1);

            if (denseIndex != last)
            {
                // swap item
                _m_pool[denseIndex] = std::move(_m_pool[last]);

                // swap handle
                const ResourceIndex movedHandle = _m_denseHandles[last];
                _m_denseHandles[denseIndex] = movedHandle;

                // fix mapping for moved handle
                _m_handleToDense[movedHandle] = denseIndex;
            }

            _m_pool.pop_back();
            _m_denseHandles.pop_back();
        }


    public:
        ResourcePool(size_t /*maxFreeIndexThreshold*/ = 500) {}

        ~ResourcePool()
        {
            if constexpr (std::is_pointer_v<T>)
            {
                for (T& ptr : _m_pool)
                {
                    delete ptr;
                    ptr = nullptr;
                }
            }
        }

        template <typename... Args>
        ResourceIndex CreateResource(Args&&... args)
        {
            static_assert(std::is_constructible_v<NonPointerT, Args...>,
                "Resource type is not constructible with the provided arguments!");

            const ResourceIndex handle = _AllocHandle();
            const ResourceIndex denseIndex = static_cast<ResourceIndex>(_m_pool.size());

            _m_handleToDense[handle] = denseIndex;
            _m_denseHandles.push_back(handle);

            if constexpr (std::is_pointer_v<T>)
            {
                _m_pool.push_back(new NonPointerT(std::forward<Args>(args)...));
            }
            else
            {
                _m_pool.emplace_back(std::forward<Args>(args)...);
            }

            return handle;
        }

        [[nodiscard]] ResourceIndex CloneResource(ResourceIndex handle) override
        {
            if constexpr (std::is_same_v<T, Texture>)
            {
                // cloning textures returns same index/handle.
                return handle;
            }
            else
            {
                ResourceIndex newHandle = CreateResource();

                NonPointerT* original = GetResourceFromPool(handle);
                NonPointerT* clone = GetResourceFromPool(newHandle);

                if (original && clone)
                    original->Clone(clone);

                return newHandle;
            }
        }

        // returns the HANDLE of the resource
        [[nodiscard]] ResourceIndex PlaceResourceInPool(T resource)
        {
            const ResourceIndex handle = _AllocHandle();
            const ResourceIndex denseIndex = static_cast<ResourceIndex>(_m_pool.size());

            _m_handleToDense[handle] = denseIndex;
            _m_denseHandles.push_back(handle);

            _m_pool.push_back(std::move(resource));
            return handle;
        }

        // returns ONLY ACTIVE items (dense pool).
        std::vector<T>* GetPoolItems()
        {
            return &_m_pool;
        }

        // Returns ACTIVE HANDLES (not dense indices). 
        std::span<const ResourceIndex> GetActiveIndexSpan()
        {
            return std::span<const ResourceIndex>(_m_denseHandles);
        }

        NonPointerT* GetResourceFromPool(ResourceIndex handle)
        {
            const ResourceIndex denseIndex = _GetDenseIndex(handle);

            if constexpr (std::is_pointer_v<T>)
                return _m_pool[denseIndex];
            else
                return &_m_pool[denseIndex];
        }

        void RemoveResourceFromPool(ResourceIndex handle) override
        {
            auto it = _m_handleToDense.find(handle);
            if (it == _m_handleToDense.end())
            {
                std::cerr << "[ResourcePool] Tried to remove invalid/non-active handle: " << handle << "\n";
                __debugbreak();
                return;
            }

            const ResourceIndex denseIndex = it->second;

            // Destroy resource payload
            if constexpr (std::is_pointer_v<T>)
            {
                delete _m_pool[denseIndex];
                _m_pool[denseIndex] = nullptr;
            }
           

            // Remove mapping for handle first (before swap touches moved handle)
            _m_handleToDense.erase(it);

            // free handle for reuse
            _m_freeHandles.push_back(handle);

            // swap-remove the dense slot
            _EraseDenseAt(denseIndex);
        }

    };
}
