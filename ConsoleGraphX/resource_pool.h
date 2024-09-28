#pragma once
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <type_traits>
#include <stdexcept>
#include "base_resource_pool.h"
#include "texture.h"

namespace ConsoleGraphX_Internal
{
    template <typename T>
    class ResourcePool : public BaseResourcePool
    {
    private:
        // maps the resource handle (as stored by entities or other systems) to the actual index in the underlying pool
        // this is useful to maintain consistent references even if the pool is resized or reallocated.
        std::unordered_map<ResourceIndex, ResourceIndex> _m_handleToPoolIndex;
        size_t _m_maxFreeIndexThreshold;

    private:
        ResourceIndex _GetCompressedIndex(ResourceIndex index)
        {
            auto it = _m_handleToPoolIndex.find(index);

            if (it == _m_handleToPoolIndex.end())
            {
                throw std::runtime_error("bad index");
            }

            return it->second;
        }

    protected:
        std::vector<T> _m_pool;
        std::queue<ResourceIndex> _m_openPoolIndexes;

        [[nodiscard]] ResourceIndex _GetOpenPoolIndex()
        {
            if (_m_openPoolIndexes.size() >= _m_maxFreeIndexThreshold)
            {
                Compress();
            }

            ResourceIndex index;
            if (!_m_openPoolIndexes.empty())
            {
                index = _m_openPoolIndexes.front();
                _m_openPoolIndexes.pop();
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
        ResourcePool(size_t maxFreeIndexThreshold = 500): _m_maxFreeIndexThreshold(maxFreeIndexThreshold)
        {}

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

        void Compress() 
        {
            if (_m_openPoolIndexes.empty()) 
            {
                return; // no need to compress if there are no open indices
            }

            // sort the indices in the queue for easier processing
            std::vector<ResourceIndex> openIndices;
            while (!_m_openPoolIndexes.empty()) 
            {
                openIndices.push_back(_m_openPoolIndexes.front());
                _m_openPoolIndexes.pop();
            }

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
                    // move valid elements to the next available spot
                    if (nextAvailableIndex != i)
                    {
                        _m_pool[nextAvailableIndex] = std::move(_m_pool[i]);

                        // update the handle-to-index map to reflect the new position
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

            // resize the pool to remove the trailing empty elements
            _m_pool.resize(nextAvailableIndex);

            // clear the queue since there are no more open spots
            std::queue<ResourceIndex>().swap(_m_openPoolIndexes);
        }



        [[nodiscard]] ResourceIndex CloneResource(ResourceIndex index) override
        {
            // temp until textures are made cloneable 
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


        [[nodiscard]] ResourceIndex PlaceResourceInPool(T resource)
        {
            ResourceIndex index = _GetOpenPoolIndex();
           
            if (index == _m_pool.size())
            {
                _m_pool.push_back(std::move(resource));
            }
            else
            {
                _m_pool[index] = std::move(resource);
            }

            return index;
        }

        std::vector<T>* GetPoolItems()
        {
            return &_m_pool;
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

            T& resource = _m_pool[cmpIndex];

            if constexpr (std::is_pointer_v<T>)
            {
                delete resource;
                resource = nullptr;
            }
            else
            {
                resource.~T();

                if constexpr (std::is_trivially_constructible<T>::value)
                {
                    std::memset(&resource, 0, sizeof(T));
                }
                else
                {
                    resource = T();
                }
            }

            _m_openPoolIndexes.push(cmpIndex);

            auto it = _m_handleToPoolIndex.find(index);

            if (it != _m_handleToPoolIndex.end())
            {
                _m_handleToPoolIndex.erase(it);
            }
        }

    };
};
