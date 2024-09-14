#include <cmath>
#include <vector>
#include "render_system.h"
#include "screen.h"
#include "sprite.h"
#include "vector2.h"
#include "vector3.h"
#include "component_manager.h"
#include "base_component_pool_impl.h"
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
        BaseComponentPoolImpl<ConsoleGraphX::Camera>* cameraPool = ComponentManager::Instance().GetComponentPool<ConsoleGraphX::Camera>();
        BaseComponentPoolImpl<ConsoleGraphX::Sprite>* spritePool = ComponentManager::Instance().GetComponentPool<ConsoleGraphX::Sprite>();
        
        std::vector<ConsoleGraphX::Camera>* cameras = cameraPool->GetPoolItems();
        std::vector<ConsoleGraphX::Sprite>* sprites = spritePool->GetPoolItems();
        for (std::vector<ConsoleGraphX::Camera>::const_iterator itCamera = cameras->begin(); itCamera != cameras->end(); ++itCamera)
        {
            const ConsoleGraphX::Camera& cam = *itCamera;

            ConsoleGraphX::Vector3 cameraPosition = cam.GetPosition();

            cameraPosition.y = std::roundf(cameraPosition.y);
            cameraPosition.x = std::roundf(cameraPosition.x);

            ConsoleGraphX::Vector2 camViewPort = cam.GetViewPort();
             
            ConsoleGraphX::Vector3 spritePosition;
            ConsoleGraphX::Vector3 relativePosition;
            for (std::vector<ConsoleGraphX::Sprite>::const_iterator itSprite = sprites->begin(); itSprite != sprites->end(); ++itSprite)
            {
                //when dereferencing sprites take a ref to avoid copying as they have dynamic memory allocated
                const ConsoleGraphX::Sprite& sprite = *itSprite;
                spritePosition = sprite.GetTransform()->GetWorldPosition();

                spritePosition.y = std::roundf(spritePosition.y);
                spritePosition.x = std::roundf(spritePosition.x);

                OverlapPoints overlapPoints;
                _CalculateEntityOverlapWithCamera(spritePosition, cameraPosition, camViewPort, sprite, overlapPoints);

                if (!_IsEntityVisibleInView(overlapPoints, sprite.Size()))
                    continue;

                relativePosition.x = spritePosition.x - cameraPosition.x;
                relativePosition.y = spritePosition.y - cameraPosition.y;

                RenderSystem::_DrawSprite_SS(relativePosition, sprite, overlapPoints);
            }
        }
    }

    void RenderSystem::_DrawSprite_SS(const ConsoleGraphX::Vector3& relEntityPosition, const ConsoleGraphX::Sprite& sprite, const OverlapPoints& overlapPoints)
    {
        CGX_VERIFY(sprite, "Null sprite");

        CHAR_INFO* buffer = Screen::GetActiveScreenBuffer_A();
        CHAR_INFO* pixels = sprite.GetPixels();

        const int spriteWidth = sprite.GetWidth();
        const int spriteHeight = sprite.GetHeight();

        const int screenWidth = Screen::GetWidth_A();

        int buffer_offset = static_cast<int>((overlapPoints.left > 0 ? 0 : relEntityPosition.x) + (overlapPoints.top > 0 ? 0 : relEntityPosition.y) * screenWidth);

        CHAR_INFO* pixelStartOffset = pixels + static_cast<int>(overlapPoints.left);
        CHAR_INFO* pixelEndOffset = pixels - static_cast<int>(overlapPoints.right);
        
        // overlapoints are rounded so casting to int wont loss any data
        for (int y = static_cast<int>(overlapPoints.top); y < spriteHeight - overlapPoints.bottom; y++)
        {
            CHAR_INFO* srcStart = pixelStartOffset + (y * spriteWidth);
            CHAR_INFO* srcEnd = pixelEndOffset + ((y + 1) * spriteWidth);

            CHAR_INFO* dest = buffer + buffer_offset;

            Screen::SetPixels_A(srcStart, srcEnd, dest);

            buffer_offset += screenWidth;
        }
    }

};