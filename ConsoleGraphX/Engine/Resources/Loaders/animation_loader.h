#pragma once

#include <string>
#include "Engine/Resources/resource_manager.h"

namespace ConsoleGraphX_Internal
{
    /**
     * @brief Loads a .cxaim animation file and registers it in the resource manager.
     *
     * @param path Path to the .cxaim file.
     * @param resourceManager Reference to the ResourceManager.
     * @param owningEntity The entity ID that will own this animation resource.
     * @return ResourceIndex The index of the created SpriteAnimation in the resource pool.
     */
    ResourceIndex LoadAnimation(const std::string& path, ResourceManager& resourceManager, size_t owningEntity);
}
