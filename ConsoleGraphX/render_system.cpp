#include <cmath>

#include <vector>
#include <unordered_set>
#include "render_system.h"
#include "entity.h"
#include "screen.h"
#include "sprite.h"
#include "vector2.h"
#include "vector3.h"
#include "component_manager.h"
#include "component_pool.h"
#include "camera.h"
#include "verify_macro.h"


/*
Parallelization: If you have a multi-core CPU, you could explore parallelizing the rendering process.
For example, you could divide the screen into smaller regions and assign a separate thread
to copy pixels for each region. This can improve performance, especially if you have a lot of sprites to render.
*/

namespace ConsoleGraphX_Internal 
{
    void RenderSystem::DrawSprites()
    {
        ComponentPool<ConsoleGraphX::Camera>* cameraPool = ComponentManager::Instance().GetComponentPool<ConsoleGraphX::Camera>();
        ComponentPool<ConsoleGraphX::Entity>* EntityPool = ComponentManager::Instance().GetComponentPool<ConsoleGraphX::Entity>();
        
        const std::vector<ConsoleGraphX::Camera>* cameras = cameraPool->GetPoolItems();
        const std::vector<ConsoleGraphX::Entity>* entities = EntityPool->GetPoolItems();
        for (std::vector<ConsoleGraphX::Camera>::const_iterator itCamera = cameras->begin(); itCamera != cameras->end(); ++itCamera)
        {
            ConsoleGraphX::Camera cam = *itCamera;

            ConsoleGraphX::Vector3 cameraPosition = cam.GetPosition();

            cameraPosition.y = std::roundf(cameraPosition.y);
            cameraPosition.x = std::roundf(cameraPosition.x);

            ConsoleGraphX::Vector2 camViewPort = cam.GetViewPort();
             
            ConsoleGraphX::Vector3 entityPosition;
            ConsoleGraphX::Vector3 relativePosition;
            for (std::vector<ConsoleGraphX::Entity>::const_iterator itEntity = entities->begin(); itEntity != entities->end(); ++itEntity)
            {
                ConsoleGraphX::Entity entity = *itEntity;
                entityPosition = entity.GetPosition(); // HIGH CPU USAGE

                entityPosition.y = std::roundf(entityPosition.y);
                entityPosition.x = std::roundf(entityPosition.x);

                // we don't check the sprite pointer as all entities in the sprite system are guaranteed to have a sprite component
                ConsoleGraphX::Sprite* sprite = entity.GetComponent<ConsoleGraphX::Sprite>();

                OverlapPoints overlapPoints;
                _CalculateEntityOverlapWithCamera(entityPosition, cameraPosition, camViewPort, sprite, overlapPoints);

                if (!_IsEntityVisibleInView(overlapPoints, sprite->Size()))
                    continue;

                relativePosition.x = entityPosition.x - cameraPosition.x;
                relativePosition.y = entityPosition.y - cameraPosition.y;

                RenderSystem::_DrawSprite_SS(relativePosition, sprite, overlapPoints);
            }
        }
    }

    void RenderSystem::_DrawSprite_SS(const ConsoleGraphX::Vector3& relEntityPosition, ConsoleGraphX::Sprite* sprite, const OverlapPoints& overlapPoints)
    {
        CGX_VERIFY(sprite, "Null sprite");

        CHAR_INFO* buffer = Screen::GetActiveScreenBuffer_A();
        CHAR_INFO* pixels = sprite->GetPixels();

        const int spriteWidth = sprite->GetWidth();
        const int spriteHeight = sprite->GetHeight();

        const int screenWidth = Screen::GetWidth_A();

        int buffer_offset = (overlapPoints.left > 0 ? 0 : relEntityPosition.x) + (overlapPoints.top > 0 ? 0 : relEntityPosition.y) * screenWidth;

        CHAR_INFO* pixelStartOffset = pixels + overlapPoints.left;
        CHAR_INFO* pixelEndOffset = pixels - overlapPoints.right;

        for (int y = overlapPoints.top; y < spriteHeight - overlapPoints.bottom; y++)
        {
            CHAR_INFO* srcStart = pixelStartOffset + (y * spriteWidth);
            CHAR_INFO* srcEnd = pixelEndOffset + ((y + 1) * spriteWidth);

            CHAR_INFO* dest = buffer + buffer_offset;

            Screen::SetPixels_A(srcStart, srcEnd, dest);

            buffer_offset += screenWidth;
        }
    }

    void RenderSystem::DrawLine(ConsoleGraphX::Vector2 origin, ConsoleGraphX::Vector2 end, int color)
    {
        CHAR_INFO s_pixel{ Screen::s_pixel, color };

        int dx = end.x - origin.x;
        int dy = end.y - origin.y;

        int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

        float xIncrement = static_cast<float>(dx) / steps;
        float yIncrement = static_cast<float>(dy) / steps;

        float x = origin.x;
        float y = origin.y;

        for (int i = 0; i <= steps; i++) {
            int ix = static_cast<int>(x);
            int iy = static_cast<int>(y);

            Screen::SetPixel_A(ix, iy, s_pixel);

            x += xIncrement;
            y += yIncrement;
        }
    }
};